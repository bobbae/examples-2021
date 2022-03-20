#define __USE_FILE_OFFSET64
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <execinfo.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ucversion.h"

#define UCMTU                1500
#define UCPORT               2666
#define UCCHUNKSZ            800

#ifdef NDEBUG
#define ucassert(expr) ((void)(0))
#else
#define ucassert(expr) ((expr) ? (void)(0) : ucfatal("assert failure, %s, %s:%d", __STRING(expr), __FILE__, __LINE__))
#endif

extern int errno;

struct ucpkthdr {
	unsigned char type;
#define UCBITMAP             0x1
#define UCFINISHED           0x2
#define UCDATA               0x3
#define UCBNIT               0x4
	unsigned char flag;
#define UCLASTFLAG           0x1
	unsigned short len;
	unsigned int pktnum;
};

struct ucbnithdr {
	unsigned int plsz, filenamesz, chunksz;
};

struct ucbnitpl {
	struct ucbnithdr hdr;
	char filename[0];
};

#define UCCACHED              0x1
#define UCFREE               0x2

struct ucbuf {
	struct ucbuf *next;
	unsigned int flag;
	struct ucpkthdr *pkt;
};

struct ucwcache {
	struct ucwcache *next;
	unsigned int count;
	struct ucbuf **wcache;
};

struct ucb {
	struct timeval currenttime, starttime, endtime, nowtime, reporttime;
	size_t totalbytes, totalpkts, okbytes, xferbytes;
	char *filename, *host, *workdir, mode;
	unsigned char *bitmap;
	struct sockaddr_in saddr;
	struct ucbuf *freelist, **rcache, **wcache;
	struct ucwcache *wcachelist;
	pthread_mutex_t bufmutex, cachemutex;
	pthread_attr_t attr;
	pthread_t filewriter, timer;
	int finished, sock, fd, port, sockbufsz, verbose, ipgfixed, sbufsz,
	    rbufsz, eof, quiet;
	unsigned int elapsedusecs, okpkts, ipg, secs, numcached, numfree,
	    invalid, duplicates, seq, lastack, iter, chunksz, wchunk, curchunk,
	    poolsz, badpkts, rate, desiredrate, bmsz, mtu, filenamesz, acked,
	    plsz, pktsz, lastseq;
};

struct ucb *ucb = NULL;

void ucbreakpoint(int code)
{
	char *ptr = NULL;

	if (code != 0) {
		fprintf(stderr, "ucbreakpoint: code %d\n", code);
		fflush(stderr);
		*ptr = 1;
	}
}

void ucprintln(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	fflush(stderr);
}

void ucterminate(int code)
{
	fflush(stdout);
	fflush(stderr);
	exit(code);
}

void ucfatal(char *msg, ...)
{
#define UCMAXTRACE          10
	void *array[UCMAXTRACE];
	char **strings;
	size_t i, size;
	va_list ap;
	int err = errno;

	fprintf(stderr, "fatal error: errno %d\n", err);
	if (err > 0)
		fprintf(stderr, "errorstr: %s", strerror(err));
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	size = backtrace(array, UCMAXTRACE);
	strings = backtrace_symbols(array, size);
	fprintf(stderr, "obtained %zd stack frames.\n", size);
	for (i = 0; i < size; i++)
		fprintf(stderr, "%s\n", strings[i]);
	fflush(stderr);
	ucbreakpoint(1);
}

void *ucalloc(size_t sz)
{
	void *xp = calloc(sz, 1);

	if (xp == NULL)
		ucfatal("ucalloc");
	return xp;
}

void ucfree(void *xp)
{
	if (xp == NULL)
		ucfatal("ucfree null");
	free(xp);
}

int ucsetsockbufs()
{
	socklen_t sz = 0;

	if (ucb->sockbufsz == 0)
		ucb->sockbufsz = 1024 * 512;
	ucb->sbufsz = ucb->sockbufsz;
	if (setsockopt
	    (ucb->sock, SOL_SOCKET, SO_SNDBUF, &ucb->sbufsz,
	     sizeof(ucb->sbufsz)) < 0)
		ucfatal("setsockopt");
	ucb->sbufsz = 0;
	sz = sizeof(ucb->sbufsz);
	if (getsockopt(ucb->sock, SOL_SOCKET, SO_SNDBUF, &ucb->sbufsz, &sz) < 0)
		ucfatal("getsockopt");
	ucb->rbufsz = ucb->sockbufsz;
	if (setsockopt
	    (ucb->sock, SOL_SOCKET, SO_RCVBUF, &ucb->rbufsz,
	     sizeof(ucb->rbufsz)) < 0)
		ucfatal("setsockopt");
	ucb->rbufsz = 0;
	sz = sizeof(ucb->rbufsz);
	if (getsockopt(ucb->sock, SOL_SOCKET, SO_RCVBUF, &ucb->rbufsz, &sz) < 0)
		ucfatal("getsockopt");
	return 0;
}

unsigned int ucusecdiff(struct timeval *t1, struct timeval *t2)
{
	unsigned int diff;

	diff = (t2->tv_sec - t1->tv_sec) * 1000000;
	diff += t2->tv_usec - t1->tv_usec;
	return diff;
}

void ucdelayusec(unsigned int usecs)
{
	if (usecs > 0)
		usleep(usecs);
}

int uctestbit(unsigned int seq)
{
	unsigned int bit = 0, byte = 0;

	seq = seq - (ucb->curchunk * ucb->chunksz);
	bit = seq & 7;
	byte = seq >> 3;
	if ((ucb->bitmap[byte] >> bit) & 1)
		return 1;
	return 0;
}

int ucsetbit(unsigned int seq)
{
	unsigned int bit = 0, byte = 0;

	seq = seq - (ucb->curchunk * ucb->chunksz);
	bit = seq & 7;
	byte = seq >> 3;
	if ((ucb->bitmap[byte] >> bit) & 1)
		return 1;
	ucb->bitmap[byte] |= 1 << bit;
	return 0;
}

void ucclearbits()
{
	memset((void *)ucb->bitmap, 0, ucb->bmsz);
	if (ucb->verbose)
		ucprintln("clear all bits");
}

void ucallocbitmap()
{
	ucb->bmsz = MAX((ucb->chunksz) >> 3, ucb->plsz);
	if (ucb->verbose)
		ucprintln("bmsz %d", ucb->bmsz);
	ucb->bitmap = (unsigned char *)ucalloc((size_t)ucb->bmsz);
	if (ucb->verbose)
		ucprintln("allocated bitmap");
}

void ucallocrcache()
{
	ucb->rcache =
	    (struct ucbuf **)ucalloc((size_t)ucb->chunksz *
				     sizeof(struct ucbuf *));
	if (ucb->verbose)
		ucprintln("allocated rcache");
}

struct ucbuf *ucallocbuf()
{
	struct ucbuf *buf = NULL;

	buf =
	    (struct ucbuf *)ucalloc((size_t)sizeof(struct ucbuf) + ucb->pktsz);
	buf->flag = UCFREE;
	buf->pkt = (struct ucpkthdr *)((char *)buf + sizeof(struct ucbuf));
	return buf;
}

void ucallocpool()
{
	unsigned int ix;
	struct ucbuf *tmpbuf = NULL, *buf;

	for (ix = 0; ix < ucb->poolsz; ix++) {
		buf = ucallocbuf();
		buf->next = tmpbuf;
		tmpbuf = buf;
	}
	ucb->freelist = tmpbuf;
	ucb->numfree = ucb->poolsz;
	if (ucb->verbose)
		ucprintln("allocated buffer pool");
}

struct ucwcache *ucallocwcache()
{
	struct ucwcache *cache = NULL;

	cache = (struct ucwcache *)ucalloc((size_t)sizeof(struct ucwcache));
	cache->count = 0;
	cache->next = NULL;
	cache->wcache =
	    (struct ucbuf **)ucalloc((size_t)ucb->chunksz *
				     sizeof(struct ucbuf *));
	if (ucb->verbose)
		ucprintln("alloc new wcache");
	return cache;
}

void ucfreewcache(struct ucwcache *cache)
{
	ucassert(cache != NULL);
	ucassert(cache->next == NULL);
	ucassert(cache->wcache != NULL);
	ucfree(cache->wcache);
	cache->wcache = NULL;
	if (ucb->verbose)
		ucprintln("free wcache");
	ucfree(cache);
}

void ucpushwcachelist(struct ucwcache *cache)
{
	struct ucwcache *np = NULL, *pp = NULL;

	if (ucb->verbose)
		ucprintln("push new wcache");
	ucassert(cache != NULL);
	ucassert(cache->next == NULL);
	ucassert(cache->wcache != NULL);
	pthread_mutex_lock(&ucb->cachemutex);
	if (ucb->wcachelist == NULL) {
		ucb->wcachelist = cache;
		pthread_mutex_unlock(&ucb->cachemutex);
		return;
	}
	pp = ucb->wcachelist;
	np = pp->next;
	while (np) {
		pp = np;
		np = np->next;
	}
	ucassert(pp != NULL);
	pp->next = cache;
	pthread_mutex_unlock(&ucb->cachemutex);
}

struct ucwcache *ucpopwcachelist()
{
	struct ucwcache *np = NULL, *pp = NULL;

	pthread_mutex_lock(&ucb->cachemutex);
	if (ucb->wcachelist == NULL) {
		pthread_mutex_unlock(&ucb->cachemutex);
		return NULL;
	}
	pp = ucb->wcachelist;
	np = pp->next;
	pp->next = NULL;
	ucb->wcachelist = np;
	pthread_mutex_unlock(&ucb->cachemutex);
	ucassert(pp->wcache != NULL);
	if (ucb->verbose)
		ucprintln("pop new wcache");
	return pp;
}

void ucinsertwcache(struct ucbuf *buf, unsigned int seq, struct ucbuf **wcache,
		    unsigned int chunk)
{
	struct ucpkthdr *pkt = NULL;

	if (ucb->verbose)
		ucprintln("cached seq %d", seq);
	pthread_mutex_lock(&ucb->cachemutex);
	if (!buf)
		ucfatal("null buf");
	buf->flag = UCCACHED;
	pkt = buf->pkt;
	ucb->numcached++;
	seq = seq - (chunk * ucb->chunksz);
	if (wcache[seq] != NULL)
		ucfatal("chunk %d cached slot %d not empty", chunk, seq);
	wcache[seq] = buf;
	pthread_mutex_unlock(&ucb->cachemutex);
}

void ucinsertrcache(struct ucbuf *buf, unsigned int seq)
{
	struct ucpkthdr *pkt = NULL;
	unsigned int oseq = seq;

	if (ucb->verbose)
		ucprintln("cached seq %d", oseq);
	pthread_mutex_lock(&ucb->cachemutex);
	if (!buf)
		ucfatal("null buf");
	buf->flag = UCCACHED;
	pkt = buf->pkt;
	ucb->numcached++;
	seq = seq - (ucb->curchunk * ucb->chunksz);
	if (ucb->rcache[seq] != NULL)
		ucfatal("cached slot %d not empty", seq);
	ucb->rcache[seq] = buf;
	pthread_mutex_unlock(&ucb->cachemutex);
}

void ucfreebuf(struct ucbuf *buf, struct ucbuf **wcache, unsigned int chunk)
{
	unsigned int seq = 0;

	pthread_mutex_lock(&ucb->bufmutex);
	if (!buf)
		ucfatal("putfree: null buf");
	if (buf->pkt == NULL)
		ucfatal("putfree: null pkt inside buf");
	seq = buf->pkt->pktnum - (chunk * ucb->chunksz);
	wcache[seq] = NULL;
	buf->flag = UCFREE;
	buf->next = ucb->freelist;
	ucb->freelist = buf;
	ucb->numfree++;
	pthread_mutex_unlock(&ucb->bufmutex);
}

void ucrecyclercache()
{
	struct ucbuf *buf = NULL;
	unsigned int seq = 0;

	if (ucb->verbose)
		ucprintln("freeall");
	pthread_mutex_lock(&ucb->bufmutex);
	for (seq = 0; seq < ucb->chunksz; seq++) {
		buf = ucb->rcache[seq];
		if (!buf)
			ucfatal("null buf");
		buf->flag = UCFREE;
		buf->next = ucb->freelist;
		ucb->freelist = buf;
		ucb->numfree++;
		ucb->rcache[seq] = NULL;
	}
	pthread_mutex_unlock(&ucb->bufmutex);
	if (ucb->verbose)
		ucprintln("recycle rcache");
}

struct ucbuf *ucgetrcache(unsigned int seq)
{
	struct ucbuf *tmpbuf = NULL;
	unsigned int oseq = seq;

	pthread_mutex_lock(&ucb->cachemutex);
	seq = seq - (ucb->curchunk * ucb->chunksz);
	tmpbuf = ucb->rcache[seq];
	if (tmpbuf == NULL) {
		pthread_mutex_unlock(&ucb->cachemutex);
		return NULL;
	}
	if (tmpbuf->flag != UCCACHED)
		ucfatal("seq %d (chunk %d seq %d) not in the cache", oseq,
			ucb->curchunk, seq);
	ucb->numcached--;
	pthread_mutex_unlock(&ucb->cachemutex);
	return tmpbuf;
}

struct ucbuf *ucgetwcache(unsigned int seq, struct ucbuf **wcache,
			  unsigned int chunk)
{
	struct ucbuf *tmpbuf = NULL;

	ucassert(wcache != NULL);
	pthread_mutex_lock(&ucb->cachemutex);
	seq = seq - (chunk * ucb->chunksz);
	tmpbuf = wcache[seq];
	if (tmpbuf == NULL) {
		pthread_mutex_unlock(&ucb->cachemutex);
		return NULL;
	}
	if (tmpbuf->flag != UCCACHED)
		ucfatal("not cached");
	ucb->numcached--;
	pthread_mutex_unlock(&ucb->cachemutex);
	return tmpbuf;
}

struct ucbuf *ucgetbuf()
{
	struct ucbuf *tmpbuf = NULL, *buf = NULL;

	pthread_mutex_lock(&ucb->bufmutex);
	if (ucb->freelist == NULL) {
		buf = ucallocbuf();
		ucassert(buf->flag == UCFREE);
		buf->next = NULL;
		pthread_mutex_unlock(&ucb->bufmutex);
		return buf;
	}
	tmpbuf = ucb->freelist;
	ucb->freelist = ucb->freelist->next;
	ucb->numfree--;
	ucassert(tmpbuf->flag == UCFREE);
	pthread_mutex_unlock(&ucb->bufmutex);
	return tmpbuf;
}

void ucprintinfo()
{
	ucprintln("peer IP %s port %d", inet_ntoa(ucb->saddr.sin_addr),
		  ucb->port);
	ucprintln("plsz %d chunksz %d filename %s", ucb->plsz, ucb->chunksz,
		  ucb->filename);
	ucprintln("sbufsz %d rbufsz %d", ucb->sbufsz, ucb->rbufsz);
}

int ucopenfile()
{
	int fd = -1;

	if (ucb->filename[0] == '-' && ucb->filename[1] == '\0')
		return (fd = 0);
	if ((fd = open(ucb->filename, O_RDONLY, 0)) < 0)
		ucfatal("open");
	return fd;
}

int uccreatefile()
{
	int fd = -1;

	if (ucb->filename[0] == '-' && ucb->filename[1] == '\0')
		return (fd = 1);
	if ((fd = open(ucb->filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0)
		ucfatal("createfile");
	return fd;
}

struct ucpkthdr *ucreadfile(unsigned int seq)
{
	ssize_t len = -1, len2 = -1, len3 = -1;
	int fd = ucb->fd;
	struct ucbuf *buf = NULL;
	struct ucpkthdr *pkt = NULL;

	if (fd < 0)
		ucfatal("bad file descriptor");
	buf = ucgetrcache(seq);
	if (buf)
		return buf->pkt;
	if (ucb->eof)
		ucfatal("eof and no entry in cache for seq %d", seq);
	buf = ucgetbuf();
	pkt = buf->pkt;
	if ((len =
	     read(fd, (char *)pkt + sizeof(struct ucpkthdr), ucb->plsz)) < 0)
		ucfatal("read error, size %d", ucb->plsz);
	pkt->len = htons(len3 = len);
	if (len < ucb->plsz) {
		if (ucb->verbose)
			ucprintln("short read %d < %d", len, ucb->plsz);
		len2 = len;
		while (len2 < ucb->plsz) {
			if ((len =
			     read(fd,
				  (char *)pkt + sizeof(struct ucpkthdr) + len2,
				  ucb->plsz - len2)) < 0)
				ucfatal("read error 2, size", ucb->plsz - len2);
			if (ucb->verbose)
				ucprintln("extra read %d", len);
			if (len == 0) {
				pkt->flag = UCLASTFLAG;
				ucb->eof++;
				ucb->lastseq = seq;
				if (ucb->verbose)
					ucprintln
					    ("last detected at seq %d len %d",
					     seq, len);
				break;
			} else if (len == ucb->plsz - len2) {
				pkt->len = htons(len3 = len + len2);
				pkt->flag = 0;
				if (ucb->verbose)
					ucprintln("extra read successful");
				break;
			} else {
				if (ucb->verbose)
					ucprintln
					    ("extra read still short: %d < %d",
					     len + len2, ucb->plsz - len2);
			}
			len2 = len2 + len;
		}
	} else {
		pkt->flag = 0;
	}
	pkt->pktnum = htonl(seq);
	pkt->type = UCDATA;
	ucinsertrcache(buf, seq);
	ucb->xferbytes += len3;
	return pkt;
}

void ucrequestbitmap()
{
	int ret = 0;
	struct ucpkthdr ucpkthdr, *pkt = &ucpkthdr;

	pkt->type = UCBITMAP;
	ret = send(ucb->sock, (void *)pkt, sizeof(struct ucpkthdr), 0);
	if (ret <= 0)
		ucfatal("send");
	if (ret != sizeof(struct ucpkthdr))
		ucfatal("send");
	if (ucb->verbose)
		ucprintln("requestbitmap");
}

void ucsendbitmap()
{
	int ret = 0;
	struct ucpkthdr ucpkthdr, *pkt = &ucpkthdr;
	unsigned int len = 0;
	struct iovec iov[2];

	iov[0].iov_base = (void *)pkt;
	iov[0].iov_len = sizeof(struct ucpkthdr);
	iov[1].iov_base = (char *)ucb->bitmap;
	iov[1].iov_len = len = MIN(ucb->plsz, ucb->bmsz);
	pkt->type = UCBITMAP;
	pkt->len = htons(len);
	pkt->pktnum = htonl(ucb->okpkts);
	ret = writev(ucb->sock, iov, 2);
	if (errno == ECONNREFUSED)
		return;
	if (ret <= 0)
		ucfatal("writev");
	if (ret != sizeof(struct ucpkthdr) + len)
		ucfatal("writev");
	if (ucb->verbose)
		ucprintln("sendbitmap ack %d", ucb->okpkts);
}

void ucreport()
{
	if (ucb->mode == 't')
		ucprintln("[%d] seq %d ack %d tot %d ipg %d cur %d",
			  ucb->secs, ucb->seq, ucb->acked, ucb->totalpkts,
			  ucb->ipg, ucb->curchunk);
	else
		ucprintln
		    ("[%d] ok %d tot %d dup %d cached %d free %d cur %d wchunk %d",
		     ucb->secs, ucb->okpkts, ucb->totalpkts, ucb->duplicates,
		     ucb->numcached, ucb->numfree, ucb->curchunk, ucb->wchunk);
}

void *uctimer(void *param)
{
	while (!ucb->finished) {
		sleep(1);
		++ucb->secs;
		if (!ucb->quiet)
			ucreport();
		if (ucb->mode == 'r' && ucb->okpkts > 0)
			ucsendbitmap();
	}
	return 0;
}

void ucwritefile(unsigned int len, void *buf)
{
	int ret;

	ret = write(ucb->fd, buf, len);
	if (ret <= 0)
		ucfatal("write error, size %d, ret %d", len, ret);
	ucb->xferbytes += ret;
}

void *ucfilewriter(void *param)
{
	struct ucbuf *buf = NULL;
	struct ucpkthdr *pkt = NULL;
	unsigned int seq = 0;
	struct ucbuf **wcache = NULL;
	struct ucwcache *wc = NULL;

	ucb->wchunk = 0;
	while ((wc = ucpopwcachelist()) == NULL) {
		if (ucb->verbose)
			ucprintln("null cache list wait...");
		usleep(100000);
	}
	wcache = wc->wcache;
	for (;;) {
		buf = ucgetwcache(seq, wcache, ucb->wchunk);
		if (buf == NULL) {
			usleep(1);
			continue;
		}
		pkt = buf->pkt;
		if (seq != pkt->pktnum)
			ucfatal("filewriter: wchunk %d, seq %d != pktnum %d",
				ucb->wchunk, seq, pkt->pktnum);
		if (ucb->verbose)
			ucprintln("writing pkt seq %d sz %d", pkt->pktnum,
				  ntohs(pkt->len));
		ucwritefile(ntohs(pkt->len),
			    (void *)((char *)pkt + sizeof(struct ucpkthdr)));
		if (ucb->lastseq == seq) {
			if (ucb->verbose)
				ucprintln("written last pkt, seq %d",
					  ucb->lastseq);
			break;
		}
		ucfreebuf(buf, wcache, ucb->wchunk);
		if ((++seq % ucb->chunksz) == 0) {
			if (ucb->verbose)
				ucprintln("free wc chunk %d", ucb->wchunk);
			ucfreewcache(wc);
			while ((wc = ucpopwcachelist()) == NULL) {
				if (ucb->verbose)
					ucprintln("null cache list wait...");
				usleep(100000);
			}
			wcache = wc->wcache;
			ucb->wchunk++;
		}
	}
	if (ucb->verbose)
		ucprintln("exiting writer");
	return 0;
}

void ucrecvfile()
{
	unsigned int delay = 0, nread = 0, seq = 0, chunk = 0, chunklim = 0;
	long sz = 0;
	int origplsz;
	struct ucbnitpl *initpl = NULL;
	struct ucpkthdr *pkt = NULL, *ipkt = NULL;
	ssize_t ret = -1;
	socklen_t saddrlen = 0;
	struct ucbuf *buf = NULL;
	time_t tm;
	char *ts;
	struct ucwcache *wc;
	struct ucbuf **wcache;

	origplsz = ucb->plsz;
	ucb->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ucb->sock < 0)
		ucfatal("socket");
	ucb->saddr.sin_family = AF_INET;
	ucb->saddr.sin_port = htons(ucb->port);
	ucb->saddr.sin_addr.s_addr = INADDR_ANY;
	if (ucsetsockbufs(ucb->sockbufsz) < 0)
		ucfatal("ucsetsockbufs");
	if (bind(ucb->sock, (struct sockaddr *)&ucb->saddr, sizeof(ucb->saddr))
	    < 0)
		ucfatal("bind");
	pkt =
	    (struct ucpkthdr *)
	    ucalloc((size_t)(sizeof(struct ucpkthdr) + ucb->plsz));
	ipkt =
	    (struct ucpkthdr *)
	    ucalloc((size_t)(sizeof(struct ucpkthdr) + ucb->plsz));
	initpl = (struct ucbnitpl *)((char *)pkt + sizeof(struct ucpkthdr));
	for (;;) {
		saddrlen = sizeof(ucb->saddr);
		ret =
		    recvfrom(ucb->sock, (void *)pkt, ucb->pktsz, 0,
			     (struct sockaddr *)&ucb->saddr, &saddrlen);
		if (ret < 0 && errno != EINTR)
			ucfatal("recvfrom");
		if (ret != ucb->pktsz)
			ucfatal("recvfrom");
		if (pkt->type != UCBNIT) {
			ucb->invalid++;
			continue;
		}
		ucb->plsz = ntohl(initpl->hdr.plsz);
		ucb->chunksz = ntohl(initpl->hdr.chunksz);

		if (ucb->filename == NULL) {
			ucb->filenamesz = ntohl(initpl->hdr.filenamesz);
			ucb->filename =
			    (char *)ucalloc((size_t)(ucb->filenamesz + 1));
			strncpy(ucb->filename, initpl->filename,
				ucb->filenamesz);
			ucb->filename[ucb->filenamesz] = '\0';
		}
		sz = pathconf(".", _PC_PATH_MAX);
		ucb->workdir = ucalloc((size_t)sz);
		if (getcwd(ucb->workdir, (size_t)sz) == NULL)
			ucfatal("getcwd");
		if (!ucb->quiet)
			ucprintinfo();
		for (delay = 0; delay < 20; delay++) {
			ret =
			    sendto(ucb->sock, (void *)pkt, ucb->pktsz, 0,
				   (struct sockaddr *)&ucb->saddr,
				   sizeof(ucb->saddr));
			if (ret <= 0)
				ucfatal("sendto");
			if (ret != ucb->pktsz)
				ucfatal("sendto");
			usleep(delay);
		}
		break;
	}
	if (ucb->plsz != origplsz) {
		ucfree(pkt);
		pkt =
		    (struct ucpkthdr *)
		    ucalloc((size_t)(sizeof(struct ucpkthdr) + ucb->plsz));
	}
	if (connect
	    (ucb->sock, (struct sockaddr *)&ucb->saddr, sizeof(ucb->saddr)) < 0)
		ucfatal("connect");
	ucb->fd = uccreatefile();
	ucallocbitmap();
	ucallocpool();
	gettimeofday(&ucb->starttime, 0);
	ucb->curchunk = chunk = 0;
	chunklim = (ucb->curchunk + 1) * ucb->chunksz;
	wc = ucallocwcache();
	ucpushwcachelist(wc);
	wcache = wc->wcache;
	pthread_create(&ucb->filewriter, &ucb->attr, ucfilewriter, NULL);
	while (!ucb->finished) {
		if (buf == NULL)
			buf = ucgetbuf();
		ipkt = buf->pkt;
		ret = recv(ucb->sock, (void *)ipkt, ucb->pktsz, 0);
		if (ret < 0 && errno != EINTR)
			ucfatal("recv");
		if (ret < nread)
			ucfatal("recv");
		switch (ipkt->type) {
		case UCBITMAP:
			if (ucb->verbose)
				ucprintln("bitmap request");
			ucsendbitmap();
			break;
		case UCFINISHED:
			ucb->finished++;
			if (ucb->verbose)
				ucprintln("finished msg received");
			break;
		case UCDATA:
			ucb->totalpkts++;
			ucb->totalbytes += ret;
			seq = ucb->seq = ipkt->pktnum = ntohl(ipkt->pktnum);
			if (ipkt->flag & UCLASTFLAG) {
				ucb->lastseq = seq;
				chunklim = seq + 1;
				if (ucb->verbose)
					ucprintln
					    ("received lastseq %d chunklim %d",
					     ucb->lastseq, chunklim);
			}
			if (seq < (ucb->curchunk * ucb->chunksz)
			    || seq >= chunklim) {
				if (ucb->verbose)
					ucprintln
					    ("chunk %d range (%d ~ %d) bad seq %d",
					     chunk, chunk * ucb->chunksz,
					     chunklim, seq);
				ucb->badpkts++;
				break;
			}
			if (ucsetbit(seq)) {
				++ucb->duplicates;
				if (ucb->verbose)
					ucprintln("duplicate seq %d", seq);
				break;
			}
			if (ucb->verbose)
				ucprintln("ok seq %d", seq);
			ucb->okbytes += ntohs(ipkt->len);
			ucinsertwcache(buf, ipkt->pktnum, wcache, chunk);
			if ((++ucb->okpkts % ucb->chunksz) == 0) {
				if (ucb->verbose)
					ucprintln("chunk %d -> %d", chunk,
						  chunk + 1);
				ucsendbitmap();
				ucb->curchunk = ++chunk;
				wc = ucallocwcache();
				ucpushwcachelist(wc);
				wcache = wc->wcache;
				ucclearbits();
				chunklim = (ucb->curchunk + 1) * ucb->chunksz;
			}
			if ((ucb->okpkts % 100) == 0)
				ucsendbitmap();	/* XXX: */
			if (ucb->okpkts == ucb->lastseq + 1) {
				if (ucb->verbose)
					ucprintln("finished, okpkts %d",
						  ucb->okpkts);
				ucb->finished++;
			}
			buf = NULL;
			break;
		default:
			break;
		}
	}
	if (ucb->verbose)
		ucprintln("finished, out of mainloop");
	ucb->finished++;
	pkt->type = UCFINISHED;
	pkt->pktnum = htonl(ucb->okpkts);
	if (ucb->verbose)
		ucprintln("sending FINISHED msg");
	for (delay = 0; delay < 100; delay++) {
		ret =
		    sendto(ucb->sock, (void *)pkt, ucb->pktsz, 0,
			   (struct sockaddr *)&ucb->saddr, sizeof(ucb->saddr));
		if (ret <= 0)
			break;
		if (ret != ucb->pktsz)
			ucfatal("sendto");
		usleep(delay);
	}
	pthread_join(ucb->filewriter, NULL);
	if (ucb->verbose) {
		time(&tm);
		ts = ctime(&tm);
		ucprintln("closing file %s, %s", ucb->filename, ts);
	}
	close(ucb->fd);
	if (ucb->verbose) {
		time(&tm);
		ts = ctime(&tm);
		ucprintln("done. %s", ts);
	}
	if (ucb->verbose)
		ucprintln("exiting receiver");
}

void ucsendblock(unsigned int seq)
{
	int ret = -1;
	struct timeval tv;
	unsigned int diff = 0;
	struct ucpkthdr *pkt = NULL;

	ucb->seq = seq;
	pkt = ucreadfile(ucb->seq);
	//ucdelayusec(ucb->ipg + 10 * ucb->iter);
	ucdelayusec(ucb->ipg);
	if (ucb->verbose && (pkt->flag & UCLASTFLAG))
		ucprintln("sending LAST msg seq %d", ntohl(pkt->pktnum));
	ret = send(ucb->sock, (void *)pkt, ucb->pktsz, 0);
	if (errno == ECONNREFUSED)
		return;
	if (ret <= 0)
		ucfatal("send");
	if (ret != ucb->pktsz)
		ucfatal("send");
	ucb->totalbytes += ret;
	ucb->totalpkts++;
	gettimeofday(&tv, 0);
	diff = ucusecdiff(&ucb->starttime, &tv);
	ucb->rate = (unsigned int)((float)ucb->totalbytes * 1000000 / diff);
	if (ucb->ipgfixed)
		return;
	if (ucb->desiredrate == 0)
		return;
	if (ucb->rate > ucb->desiredrate)
		ucb->ipg++;
	else if (ucb->ipg > 0)
		ucb->ipg--;
}

void ucgetinput(struct ucpkthdr *ipkt, int nread)
{
	int ret = -1;
	unsigned int len = 0, acked = 0;
	struct iovec iov[2];

	ret = recv(ucb->sock, (void *)ipkt, sizeof(struct ucpkthdr), MSG_PEEK);
	if (ret < 0 && errno != EINTR)
		ucfatal("recv");
	if (ret < sizeof(struct ucpkthdr))
		ucfatal("recv");
	switch (ipkt->type) {
	case UCFINISHED:
		ret = recv(ucb->sock, (void *)ipkt, nread, 0);
		if (ret < 0 && errno != EINTR)
			ucfatal("recv");
		if (ucb->verbose)
			ucprintln("finished, received FINISHED msg.");
		ucb->finished++;
		ucb->acked = ntohl(ipkt->pktnum);
		break;
	case UCBITMAP:
		len = ntohs(ipkt->len);
		acked = ntohl(ipkt->pktnum);
		if (acked <= ucb->acked) {
			ret = recv(ucb->sock, (void *)ipkt, nread, 0);
			if (ret < 0 && errno != EINTR)
				ucfatal("recv");
			ipkt->type = 0;
			break;
		}
		ucb->acked = acked;
		iov[0].iov_base = (void *)ipkt;
		iov[0].iov_len = sizeof(struct ucpkthdr);
		iov[1].iov_base = (void *)ucb->bitmap;
		iov[1].iov_len = len;
		ret = readv(ucb->sock, iov, 2);
		if (ret <= 0)
			ucfatal("readv");
		if (ret < len + sizeof(struct ucpkthdr))
			ucfatal("readv");
		break;
	default:
		ret = recv(ucb->sock, (void *)ipkt, nread, 0);
		if (ret < 0 && errno != EINTR)
			ucfatal("recv");
		if (ret < nread)
			ucfatal("recv");
		break;
	}
}

unsigned int ucadvseq(unsigned int seq, unsigned int limit)
{
	ucassert(seq <= limit);
	if (seq == ucb->lastseq || ++seq == limit) {
		seq = ucb->curchunk * ucb->chunksz;
		ucb->iter++;
	}
	return seq;
}

void ucxmitfile()
{
	int ix = 0, nread = 0, connected = 0, chunkdone = 0;
	unsigned int flen = 0, delay = 0, seq = 0, chunk = 0, chunklim = 0;
	struct ucpkthdr *pkt = NULL, *ipkt = NULL;
	struct sockaddr_in rsaddr;
	ssize_t ret = -1;
	struct ucbnitpl *initpl = NULL;
	socklen_t saddrlen = 0;
	long sz = 0;

	ucb->fd = ucopenfile();
	ucb->sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ucb->sock < 0)
		ucfatal("socket");
	ucb->saddr.sin_family = AF_INET;
	ucb->saddr.sin_port = htons(ucb->port);
	ucb->saddr.sin_addr.s_addr = inet_addr(ucb->host);
	memcpy((void *)&rsaddr, (void *)&ucb->saddr, sizeof(ucb->saddr));
	if (ucsetsockbufs(ucb->sockbufsz) < 0)
		ucfatal("ucsetsockbufs");
	pkt =
	    (struct ucpkthdr *)
	    ucalloc((size_t)(sizeof(struct ucpkthdr) + ucb->plsz));
	ipkt =
	    (struct ucpkthdr *)
	    ucalloc((size_t)(sizeof(struct ucpkthdr) + ucb->plsz));
	initpl = (struct ucbnitpl *)((char *)pkt + sizeof(struct ucpkthdr));
	pkt->type = UCBNIT;
	initpl->hdr.plsz = htonl(ucb->plsz);
	initpl->hdr.chunksz = htonl(ucb->chunksz);
	flen = strlen(ucb->filename);
	initpl->hdr.filenamesz = htonl(flen);
	strncpy(initpl->filename, ucb->filename, flen);
	initpl->filename[flen] = '\0';
	sz = pathconf(".", _PC_PATH_MAX);
	ucb->workdir = ucalloc((size_t)sz);
	if (getcwd(ucb->workdir, (size_t)sz) == NULL)
		ucfatal("getcwd");
	for (delay = 0; delay < 1000; delay++) {
		for (ix = 0; ix < 20; ix++) {
			ret =
			    sendto(ucb->sock, (void *)pkt, ucb->pktsz, 0,
				   (struct sockaddr *)&ucb->saddr,
				   sizeof(ucb->saddr));
			if (ret <= 0)
				ucfatal("sendto");
			if (ret != ucb->pktsz)
				ucfatal("sendto");
			usleep(delay);
			nread = 0;
			if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
				ucfatal("ioctl");
			if (nread >= ucb->pktsz)
				break;
		}
		if (nread < ucb->pktsz)
			continue;
		saddrlen = sizeof(rsaddr);
		ret =
		    recvfrom(ucb->sock, (void *)ipkt, ucb->pktsz, 0,
			     (struct sockaddr *)&rsaddr, &saddrlen);
		if (ret < 0 && errno != EINTR)
			ucfatal("recvfrom");
		if (ret != ucb->pktsz)
			ucfatal("recvfrom");
		if (ipkt->type != UCBNIT)
			continue;
		if (memcmp((void *)pkt, (void *)ipkt, ucb->pktsz) == 0) {
			if (connect
			    (ucb->sock, (struct sockaddr *)&rsaddr,
			     sizeof(rsaddr)) < 0)
				ucfatal("connect");
			connected++;
			break;
		}
		fprintf(stderr,
			"error: remote init pkt echo does not match.\n");
		fflush(stderr);
		ucterminate(1);
	}
	if (!connected) {
		fprintf(stderr, "cannot contact peer %s\n", ucb->host);
		fflush(stderr);
		ucterminate(1);
	}
	if (!ucb->quiet)
		ucprintinfo();
	ucallocbitmap();
	ucallocpool();
	ucallocrcache();
	gettimeofday(&ucb->starttime, 0);
	ucb->reporttime = ucb->starttime;
	chunk = 0;
	for (;;) {
		seq = chunk * ucb->chunksz;
		if (ucb->verbose)
			ucprintln("start sending chunk %d, range %d ~ %d",
				  chunk, seq, (chunk + 1) * ucb->chunksz);
		chunkdone = 0;
		ucb->iter = 0;
		chunklim = seq + ucb->chunksz;
		while (!chunkdone) {
			nread = 0;
			if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
				ucfatal("fionread");
			while (nread >= sizeof(struct ucpkthdr)) {
				ucgetinput(ipkt, nread);
				if (ipkt->type == UCFINISHED) {
					ucb->acked = ntohs(ipkt->pktnum);
					if (ucb->verbose)
						ucprintln
						    ("finished, received FINISHED");
					break;
				}
				if (ipkt->type == UCBITMAP) {
					if (ntohl(ipkt->pktnum) ==
					    (chunk + 1) * ucb->chunksz) {
						if (ucb->verbose)
							ucprintln
							    ("chunk %d done",
							     chunk);
						chunkdone++;
						break;
					}
				}
				nread = 0;
				if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
					ucfatal("fionread");
			}
			if (ucb->finished) {
				if (ucb->verbose)
					ucprintln("finished, break 1");
				break;
			}
			if (chunkdone) {
				if (ucb->verbose)
					ucprintln("chunk %d done", chunk);
				break;
			}
			ucassert(seq >= chunk * ucb->chunksz);
			ucassert(seq < (chunk + 1) * ucb->chunksz);
			if (ucb->iter > 0 && uctestbit(seq)) {
				seq = ucadvseq(seq, chunklim);
				continue;
			}
			if (ucb->verbose)
				ucprintln("sending seq %d", seq);
			ucsendblock(seq);
			seq = ucadvseq(seq, chunklim);
			if (ucb->verbose)
				ucprintln("next seq %d", seq);
		}
		if (ucb->finished) {
			if (ucb->verbose)
				ucprintln("finished, break 2");
			break;
		}
		chunkdone = 0;
		ucb->curchunk = ++chunk;
		if (ucb->verbose)
			ucprintln("chunk %d -> %d", chunk - 1, chunk);
		ucrecyclercache();
		ucclearbits();
	}
	ucfree(pkt);
	ucfree(ipkt);
	if (ucb->verbose)
		ucprintln("exiting transmitter.");
}

void ucusage(char *progname)
{
	fprintf(stderr, "%s [options]\n", progname);
	fprintf(stderr, "[-h host] required if sender; default: receiver\n");
	fprintf(stderr, "[-r desired_rate] rate in B/s; default: unlimited\n");
	fprintf(stderr, "[-d ipg] delay between packets in usec; default: 0\n");
	fprintf(stderr, "[-f testfile] filename; default: 'testfile'\n");
	fprintf(stderr, "[-p 2666] port; default: 2666\n");
	fprintf(stderr, "[-m mtu] max trans unit; default: 1500\n");
	fprintf(stderr, "[-v] enable verbose ouput; default: off\n");
	ucterminate(1);
}

unsigned int ucgetint(char *str)
{
	char *p = str;

	while (*p) {
		if (!isdigit(*p)) {
			ucprintln("invalid integer input %s", str);
			ucfatal("input error");
		}
		p++;
	}
	return atoi(str);
}

void ucgetoptions(int argc, char *argv[], char *optstring)
{
	int opt;

	while ((opt = getopt(argc, argv, optstring)) != -1) {
		switch (opt) {
		case 'q':
			ucb->quiet = 1;
			break;
		case 'r':
			ucb->desiredrate = ucgetint(optarg);
			break;
		case 'd':
			ucb->ipg = ucgetint(optarg);
			ucb->ipgfixed++;
			break;
		case 'm':
			ucb->mtu = ucgetint(optarg);
			break;
		case 'f':
			ucb->filename = optarg;
			break;
		case 'v':
			ucb->verbose++;
			break;
		case 'h':
			ucb->host = optarg;
			ucb->mode = 't';
			break;
		case 'p':
			ucb->port = ucgetint(optarg);
			break;
		default:
			ucusage(argv[0]);
		}
	}
}

int ucmain(int argc, char *argv[])
{
	unsigned int tdiff = 0, tmin = 0, tsec = 0;
	time_t tm;
	char *ts = NULL, *optstring = "c:d:h:f:m:p:r:s:vq";

	ucb = (struct ucb *)ucalloc(sizeof(struct ucb));
	ucb->quiet = 0;
	ucb->lastseq = 0xffffffff;
	ucb->mode = 'r';
	ucb->port = UCPORT;
	ucb->mtu = UCMTU;
	ucb->filename = NULL;
	ucgetoptions(argc, argv, optstring);
	if (ucb->filename == NULL)
		ucb->filename = "testfile";
	pthread_attr_init(&ucb->attr);
	pthread_create(&ucb->timer, &ucb->attr, uctimer, NULL);
	if (ucb->plsz == 0)
		ucb->plsz = ucb->mtu - 20 - 8 - sizeof(struct ucpkthdr);
	ucb->chunksz = ucb->plsz << 3;
	ucb->pktsz = ucb->plsz + sizeof(struct ucpkthdr);
	ucb->poolsz = ucb->chunksz;
	if (!ucb->quiet) {
		time(&tm);
		ts = ctime(&tm);
		ucprintln("%s version %s , start time: %s process id %d",
			  argv[0], ucversion, ts, getpid());
	}
	if (ucb->mode == 't') {
		if (!ucb->quiet)
			ucprintln("Sending...");
		ucxmitfile();
	} else {
		if (!ucb->quiet)
			ucprintln("Receiving...");
		pthread_mutex_init(&ucb->bufmutex, NULL);
		pthread_mutex_init(&ucb->cachemutex, NULL);
		ucrecvfile();
	}
	gettimeofday(&ucb->endtime, 0);
	tdiff = ucusecdiff(&ucb->starttime, &ucb->endtime);
	tmin = tdiff / 1000000 / 60;
	tsec = tdiff / 1000000 % 60;
	if (!ucb->quiet) {
		unsigned long long val;
		time(&tm);
		ts = ctime(&tm);
		ucprintln("%lld bytes transferred", ucb->xferbytes);
		ucprintln("end time: %s", ts, getpid());
		ucprintln("elapsed time: %.2d min %.2d secs", tmin, tsec);
		val = ucb->seq;
		val = val * ucb->plsz;
		ucprintln("%lld bytes per second", val / (tdiff / 1000000));
	}
	fflush(stdout);
	fflush(stderr);
	ucterminate(0);
	return 0;
}

#ifndef __COMPILE_LIBRARY__
int main(int argc, char *argv[])
{
	return ucmain(argc, argv);
}
#endif
