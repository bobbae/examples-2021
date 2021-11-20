#define __USE_FILE_OFFSET64
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include "argparse.h"
#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#define UC_PROG_NAME "ucat"
#define UC_VERSION "1.0.0"
#define UC_MTU 1500
#define UC_PORT 2666
#define UC_CHUNK_SIZE 800

#ifdef NDEBUG
#define uc_assert(expr) ((void)(0))
#else
#define uc_assert(expr)                      \
    ((expr) ? (void)(0)                      \
            : uc_fatalf(__LINE__,            \
                "assert failure, %s, %s:%d", \
                __STRING(expr),              \
                __FILE__,                    \
                __LINE__))
#endif
#define UC_FATAL(msg) uc_fatalf(__LINE__, msg)
#define UC_EXIT(msg, code) uc_exit(__LINE__, msg, code)

extern int errno;

#define UC_BITMAP 1
#define UC_FINISHED 2
#define UC_DATA 3
#define UC_INIT 4
#define UC_PING 5

#define UC_LAST_FLAG 0x1
#define UC_ACK_FLAG 0x2
#define UC_CONNECT_FLAG 0x4
#define UC_ACK_WAIT_FLAG 0x8

struct uc_pkt_hdr {
    unsigned char type;
    unsigned char flags;
    unsigned short len;
    unsigned int pktnum;
};

struct uc_init_hdr {
    unsigned int plsz, filenamesz, chunksz;
};

struct uc_init_pl {
    struct uc_init_hdr hdr;
    char filename[0];
};

#define UC_CACHED 0x1
#define UC_FREE 0x2

struct uc_buf {
    struct uc_buf* next;
    unsigned int flags;
    struct uc_pkt_hdr* pkt;
};

struct uc_write_cache {
    struct uc_write_cache* next;
    unsigned int count;
    struct uc_buf** wcache;
};

struct uc_time_data {
    int result;
    int type;
    int flags;
    int timeout;
    int maxdelay;
    struct sockaddr_in addr;
    struct uc_pkt_hdr* pkt;
};

#define UC_ACK_OK 201
#define UC_RECV_OK 202

#define UC_ERR_TIMEOUT 408
#define UC_ERR_INVALID_SIZE 409
#define UC_ERR_INVALID_TYPE 410
#define UC_ERR_INVALID_FLAGS 411
#define UC_ERR_DISCONNECT 412

struct ucb {
    struct timeval currenttime, starttime, endtime, nowtime, reporttime;
    size_t totalbytes, totalpkts, okbytes, xferbytes;
    char *filename, *host, *workdir, mode;
    unsigned char* bitmap;
    struct sockaddr_in saddr;
    struct uc_buf *freelist, **rcache, **wcache;
    struct uc_write_cache* wcachelist;
    pthread_mutex_t bufmutex, cachemutex;
    pthread_attr_t attr;
    pthread_t filewriter, timer;
    int debug, finished, sock, fd, port, sockbufsz, verbose, ipgfixed,
        sbufsz, rbufsz, eof, quiet;
    unsigned int elapsedusecs, okpkts, ipg, secs, numcached, numfree,
        invalid, duplicates, seq, lastack, iter, chunksz, wchunk,
        curchunk, poolsz, badpkts, rate, desiredrate, bmsz, mtu,
        filenamesz, acked, plsz, pktsz, lastseq, perms;
};

struct ucb* ucb = NULL;

char* ntoa(struct sockaddr_in saddr)
{
    static char sbuf[240];
    sprintf(sbuf, "%s:%d", inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
    return sbuf;
}

void uc_breakpoint(int code)
{
    char* ptr = NULL;

    if (code != 0) {
        fprintf(stderr, "uc_breakpoint: code %d\n", code);
        fflush(stderr);
        *ptr = 1;
    }
    exit(1);
}

void uc_println(char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "::: ");
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
    fflush(stderr);
}

void uc_terminate(int code)
{
    fflush(stdout);
    fflush(stderr);
    exit(code);
}

void uc_exit(int line, char* msg, int code)
{
    uc_println("::: Exiting, line %d, %s, code %d", line, msg, code);
    uc_terminate(code);
}

void uc_fatalf(int line, char* msg, ...)
{
#define UCMAXTRACE 10
    void* array[UCMAXTRACE];
    char** strings;
    size_t i, size;
    va_list ap;
    int err = errno;

    fprintf(stderr, "::: Exiting intentionally, fatal error: line %d, msg %s, errno %d, %s\n", line, msg, err, strerror(err));

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);

    fprintf(stderr, "\n");
#ifdef BACKTRACE
    size = backtrace(array, UCMAXTRACE);
    strings = backtrace_symbols(array, size);
    fprintf(stderr, "::: obtained %zd stack frames.\n", size);
    for (i = 0; i < size; i++)
        fprintf(stderr, "::: %s\n", strings[i]);
    fflush(stderr);
#endif
    if (ucb->debug)
        uc_breakpoint(1);
    exit(1);
}

void* uc_alloc(size_t sz)
{
    void* xp = calloc(sz, 1);

    if (xp == NULL)
        UC_FATAL("uc_alloc");
    return xp;
}

void uc_free(void* xp)
{
    if (xp == NULL)
        UC_FATAL("uc_free null");
    free(xp);
}

int uc_set_sock_bufs()
{
    socklen_t sz = 0;

    assert(ucb->sockbufsz > 0);
    ucb->sbufsz = 0;
    sz = sizeof(ucb->sbufsz);
    if (getsockopt(ucb->sock, SOL_SOCKET, SO_SNDBUF, &ucb->sbufsz, &sz) < 0)
        UC_FATAL("getsockopt");

    if (ucb->sbufsz < ucb->sockbufsz) {
        if (ucb->verbose)
            uc_println("sbufsz %d -> sockbufsz %d ", ucb->sbufsz, ucb->sockbufsz);
        ucb->sbufsz = ucb->sockbufsz;
    }

    if (setsockopt(
            ucb->sock, SOL_SOCKET, SO_SNDBUF, &ucb->sbufsz, sizeof(ucb->sbufsz))
        < 0)
        UC_FATAL("setsockopt");

    ucb->rbufsz = 0;
    sz = sizeof(ucb->rbufsz);
    if (getsockopt(ucb->sock, SOL_SOCKET, SO_RCVBUF, &ucb->rbufsz, &sz) < 0)
        UC_FATAL("getsockopt");

    if (ucb->rbufsz < ucb->sockbufsz) {
        if (ucb->verbose)
            uc_println("rbufsz %d -> sockbufsz %d ", ucb->rbufsz, ucb->sockbufsz);
        ucb->rbufsz = ucb->sockbufsz;
    }
    if (setsockopt(
            ucb->sock, SOL_SOCKET, SO_RCVBUF, &ucb->rbufsz, sizeof(ucb->rbufsz))
        < 0)
        UC_FATAL("setsockopt");
    if (ucb->verbose) {
        uc_println("rbufsz %d, sbufsz %d", ucb->rbufsz, ucb->sbufsz);
    }
    return 0;
}

unsigned int
uc_usec_diff(struct timeval* t1, struct timeval* t2)
{
    unsigned int diff;

    diff = (t2->tv_sec - t1->tv_sec) * 1000000;
    diff += t2->tv_usec - t1->tv_usec;
    return diff;
}

void uc_delay_usec(unsigned int usecs)
{
    if (usecs > 0)
        usleep(usecs);
}

int uc_test_bit(unsigned int seq)
{
    unsigned int bit = 0, byte = 0;

    seq = seq - (ucb->curchunk * ucb->chunksz);
    bit = seq & 7;
    byte = seq >> 3;
    if ((ucb->bitmap[byte] >> bit) & 1)
        return 1;

    return 0;
}

int uc_set_bit(unsigned int seq)
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

void uc_clear_bits()
{
    memset((void*)ucb->bitmap, 0, ucb->bmsz);
    if (ucb->verbose)
        uc_println("clear all bits");
}

void uc_alloc_bitmap()
{
    ucb->bmsz = MAX((ucb->chunksz) >> 3, ucb->plsz);

    if (ucb->verbose)
        uc_println("bmsz %d", ucb->bmsz);

    ucb->bitmap = (unsigned char*)uc_alloc((size_t)ucb->bmsz);

    if (ucb->verbose)
        uc_println("allocated bitmap");
}

void uc_alloc_read_cache()
{
    ucb->rcache = (struct uc_buf**)uc_alloc((size_t)ucb->chunksz * sizeof(struct uc_buf*));
    if (ucb->verbose)
        uc_println("allocated rcache");
}

struct uc_buf*
uc_alloc_buf()
{
    struct uc_buf* buf = NULL;

    buf = (struct uc_buf*)uc_alloc((size_t)sizeof(struct uc_buf) + ucb->pktsz);
    buf->flags = UC_FREE;
    buf->pkt = (struct uc_pkt_hdr*)((char*)buf + sizeof(struct uc_buf));

    return buf;
}

void uc_alloc_pool()
{
    unsigned int ix;
    struct uc_buf *tmpbuf = NULL, *buf;

    for (ix = 0; ix < ucb->poolsz; ix++) {
        buf = uc_alloc_buf();
        buf->next = tmpbuf;
        tmpbuf = buf;
    }
    ucb->freelist = tmpbuf;
    ucb->numfree = ucb->poolsz;
    if (ucb->verbose)
        uc_println("allocated buffer pool");
}

struct uc_write_cache*
uc_alloc_write_cache()
{
    struct uc_write_cache* cache = NULL;

    cache = (struct uc_write_cache*)uc_alloc((size_t)sizeof(struct uc_write_cache));
    cache->count = 0;
    cache->next = NULL;
    cache->wcache = (struct uc_buf**)uc_alloc((size_t)ucb->chunksz * sizeof(struct uc_buf*));

    if (ucb->verbose)
        uc_println("alloc new wcache");

    return cache;
}

void uc_free_write_cache(struct uc_write_cache* cache)
{
    uc_assert(cache != NULL);
    uc_assert(cache->next == NULL);
    uc_assert(cache->wcache != NULL);

    uc_free(cache->wcache);
    cache->wcache = NULL;

    if (ucb->verbose)
        uc_println("free wcache");

    uc_free(cache);
}

void uc_push_write_cache_list(struct uc_write_cache* cache)
{
    struct uc_write_cache *np = NULL, *pp = NULL;

    if (ucb->verbose)
        uc_println("push new wcache");

    uc_assert(cache != NULL);
    uc_assert(cache->next == NULL);
    uc_assert(cache->wcache != NULL);

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

    uc_assert(pp != NULL);

    pp->next = cache;
    pthread_mutex_unlock(&ucb->cachemutex);
}

struct uc_write_cache*
uc_pop_write_cache_list()
{
    struct uc_write_cache *np = NULL, *pp = NULL;

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

    uc_assert(pp->wcache != NULL);

    if (ucb->verbose)
        uc_println("pop new wcache");

    return pp;
}

void uc_insert_write_cache(struct uc_buf* buf,
    unsigned int seq,
    struct uc_buf** wcache,
    unsigned int chunk)
{
    struct uc_pkt_hdr* pkt = NULL;

    if (ucb->verbose)
        uc_println("cached write seq %d", seq);

    pthread_mutex_lock(&ucb->cachemutex);

    if (!buf)
        UC_FATAL("null buf");

    buf->flags = UC_CACHED;
    pkt = buf->pkt;
    ucb->numcached++;
    seq = seq - (chunk * ucb->chunksz);

    if (wcache[seq] != NULL) {
        uc_println("chunk %d cached slot %d not empty", chunk, seq);
        UC_FATAL("uc_insert_write_cache");
    }

    wcache[seq] = buf;
    pthread_mutex_unlock(&ucb->cachemutex);
}

void uc_insert_read_cache(struct uc_buf* buf, unsigned int seq)
{
    struct uc_pkt_hdr* pkt = NULL;
    unsigned int oseq = seq;

    if (ucb->verbose)
        uc_println("cached seq %d", oseq);

    pthread_mutex_lock(&ucb->cachemutex);

    if (!buf)
        UC_FATAL("null buf");

    buf->flags = UC_CACHED;
    pkt = buf->pkt;
    ucb->numcached++;
    seq = seq - (ucb->curchunk * ucb->chunksz);

    if (ucb->rcache[seq] != NULL) {
        uc_println("cached slot %d not empty", seq);
        UC_FATAL("uc_insert_read_cache");
    }

    ucb->rcache[seq] = buf;
    pthread_mutex_unlock(&ucb->cachemutex);
}

void uc_free_buf(struct uc_buf* buf, struct uc_buf** wcache, unsigned int chunk)
{
    unsigned int seq = 0;

    pthread_mutex_lock(&ucb->bufmutex);
    if (!buf)
        UC_FATAL("putfree: null buf");

    if (buf->pkt == NULL)
        UC_FATAL("putfree: null pkt inside buf");

    seq = buf->pkt->pktnum - (chunk * ucb->chunksz);
    wcache[seq] = NULL;
    buf->flags = UC_FREE;
    buf->next = ucb->freelist;
    ucb->freelist = buf;
    ucb->numfree++;

    pthread_mutex_unlock(&ucb->bufmutex);
}

void uc_recycle_read_cache()
{
    struct uc_buf* buf = NULL;
    unsigned int seq = 0;

    if (ucb->verbose)
        uc_println("recycle rcache. freeall");

    pthread_mutex_lock(&ucb->bufmutex);

    for (seq = 0; seq < ucb->chunksz; seq++) {
        buf = ucb->rcache[seq];
        if (!buf) {
            //if (ucb->verbose) {
            //   uc_println("null buf");
            //}
            continue;
        }
        buf->flags = UC_FREE;
        buf->next = ucb->freelist;
        ucb->freelist = buf;
        ucb->numfree++;
        ucb->rcache[seq] = NULL;
    }

    pthread_mutex_unlock(&ucb->bufmutex);

    if (ucb->verbose)
        uc_println("recycle rcache done");
}

struct uc_buf*
uc_get_read_cache(unsigned int seq)
{
    struct uc_buf* tmpbuf = NULL;
    unsigned int oseq = seq;

    pthread_mutex_lock(&ucb->cachemutex);
    seq = seq - (ucb->curchunk * ucb->chunksz);
    tmpbuf = ucb->rcache[seq];

    if (tmpbuf == NULL) {
        pthread_mutex_unlock(&ucb->cachemutex);
        return NULL;
    }
    if (tmpbuf->flags != UC_CACHED) {
        uc_println(
            "seq %d (chunk %d seq %d) not in the cache",
            oseq,
            ucb->curchunk,
            seq);
        UC_FATAL("uc_get_read_cache");
    }

    ucb->numcached--;
    pthread_mutex_unlock(&ucb->cachemutex);
    return tmpbuf;
}

struct uc_buf*
uc_get_write_cache(unsigned int seq, struct uc_buf** wcache, unsigned int chunk)
{
    struct uc_buf* tmpbuf = NULL;

    uc_assert(wcache != NULL);
    pthread_mutex_lock(&ucb->cachemutex);
    seq = seq - (chunk * ucb->chunksz);
    tmpbuf = wcache[seq];
    if (tmpbuf == NULL) {
        pthread_mutex_unlock(&ucb->cachemutex);
        return NULL;
    }
    if (tmpbuf->flags != UC_CACHED)
        UC_FATAL("not cached");
    ucb->numcached--;
    pthread_mutex_unlock(&ucb->cachemutex);
    return tmpbuf;
}

struct uc_buf*
uc_get_buf()
{
    struct uc_buf *tmpbuf = NULL, *buf = NULL;

    pthread_mutex_lock(&ucb->bufmutex);
    if (ucb->freelist == NULL) {
        buf = uc_alloc_buf();
        uc_assert(buf->flags == UC_FREE);
        buf->next = NULL;
        pthread_mutex_unlock(&ucb->bufmutex);
        return buf;
    }
    tmpbuf = ucb->freelist;
    ucb->freelist = ucb->freelist->next;
    ucb->numfree--;
    uc_assert(tmpbuf->flags == UC_FREE);
    pthread_mutex_unlock(&ucb->bufmutex);
    return tmpbuf;
}

void uc_print_info()
{
    uc_println("peer IP %s port %d", inet_ntoa(ucb->saddr.sin_addr), ucb->port);
    uc_println(
        "plsz %d chunksz %d filename %s", ucb->plsz, ucb->chunksz, ucb->filename);
    uc_println("sbufsz %d rbufsz %d", ucb->sbufsz, ucb->rbufsz);
}

int uc_open_file()
{
    int fd = -1;

    if (ucb->filename[0] == '-' && ucb->filename[1] == '\0')
        return (fd = 0);

    if ((fd = open(ucb->filename, O_RDONLY, 0)) < 0)
        UC_FATAL("open");

    return fd;
}

int uc_create_file()
{
    int fd = -1;

    if (ucb->filename[0] == '-' && ucb->filename[1] == '\0')
        return (fd = 1);

    if ((fd = open(ucb->filename, O_WRONLY | O_TRUNC | O_CREAT, 0644)) < 0)
        UC_FATAL("createfile");

    return fd;
}

struct uc_pkt_hdr*
uc_read_file(unsigned int seq)
{
    ssize_t len = -1, len2 = -1, len3 = -1;
    int fd = ucb->fd;
    struct uc_buf* buf = NULL;
    struct uc_pkt_hdr* pkt = NULL;

    if (fd < 0)
        UC_FATAL("bad file descriptor");

    buf = uc_get_read_cache(seq);

    if (buf)
        return buf->pkt;

    if (ucb->eof) {
        uc_println("eof and no entry in cache for seq %d", seq);
        UC_FATAL("uc_read_file eof");
    }

    buf = uc_get_buf();
    pkt = buf->pkt;

    if ((len = read(fd, (char*)pkt + sizeof(struct uc_pkt_hdr), ucb->plsz)) < 0) {
        uc_println("read error, size %d", ucb->plsz);
        UC_FATAL("uc_read_file error");
    }

    pkt->len = htons(len3 = len);
    if (len < ucb->plsz) {
        if (ucb->verbose)
            uc_println("short read %d < %d", len, ucb->plsz);

        len2 = len;
        while (len2 < ucb->plsz) {
            if ((len = read(fd,
                     (char*)pkt + sizeof(struct uc_pkt_hdr) + len2,
                     ucb->plsz - len2))
                < 0) {
                uc_println("read error 2, size", ucb->plsz - len2);
                UC_FATAL("read error");
            }

            if (ucb->verbose)
                uc_println("extra read %d", len);

            if (len == 0) {
                pkt->flags = UC_LAST_FLAG;
                ucb->eof++;
                ucb->lastseq = seq;

                if (ucb->verbose)
                    uc_println("last detected at seq %d len %d", seq, len);
                break;
            } else if (len == ucb->plsz - len2) {
                pkt->len = htons(len3 = len + len2);
                pkt->flags = 0;

                if (ucb->verbose)
                    uc_println("extra read successful");
                break;
            } else {
                if (ucb->verbose)
                    uc_println(
                        "extra read still short: %d < %d", len + len2, ucb->plsz - len2);
            }
            len2 = len2 + len;
        }
    } else {
        pkt->flags = 0;
    }
    pkt->pktnum = htonl(seq);
    pkt->type = UC_DATA;
    uc_insert_read_cache(buf, seq);
    ucb->xferbytes += len3;
    return pkt;
}

int uc_send_bitmap()
{
    int ret = 0;
    struct uc_pkt_hdr uc_pkt_hdr, *pkt = &uc_pkt_hdr;
    unsigned int len = 0;
    struct iovec iov[2];

    iov[0].iov_base = (void*)pkt;
    iov[0].iov_len = sizeof(struct uc_pkt_hdr);
    iov[1].iov_base = (char*)ucb->bitmap;
    iov[1].iov_len = len = MIN(ucb->plsz, ucb->bmsz);
    pkt->type = UC_BITMAP;
    pkt->len = htons(len);
    pkt->pktnum = htonl(ucb->okpkts);
    ret = writev(ucb->sock, iov, 2);
    if (errno == ETIMEDOUT) {
        if (ucb->verbose) {
            uc_println("sendbitmap ETIMEDOUT");
        }
        return 0;
    }
    if (errno != 0) {
        if (errno == ECONNREFUSED) {
            if (ucb->verbose) {
                uc_println("sendbitmap connection refused");
            }
            return errno;
        }

        if (ucb->verbose) {
            uc_println("sendbitmap errno %d, %s", errno, strerror(errno));
        }
        return errno;
    }
    if (ret <= 0)
        UC_FATAL("writev");
    if (ret != sizeof(struct uc_pkt_hdr) + len)
        UC_FATAL("writev");
    if (ucb->verbose)
        uc_println("sendbitmap ack %d", ucb->okpkts);
    return 0;
}

void uc_report()
{
    if (ucb->mode == 't')
        uc_println("[%d] seq %d ack %d tot %d ipg %d cur %d",
            ucb->secs,
            ucb->seq,
            ucb->acked,
            ucb->totalpkts,
            ucb->ipg,
            ucb->curchunk);
    else
        uc_println("[%d] ok %d tot %d dup %d cached %d free %d cur %d wchunk %d",
            ucb->secs,
            ucb->okpkts,
            ucb->totalpkts,
            ucb->duplicates,
            ucb->numcached,
            ucb->numfree,
            ucb->curchunk,
            ucb->wchunk);
}

void* uc_timer(void* param)
{
    int ret;

    while (!ucb->finished) {
        sleep(1);
        ++ucb->secs;
        if (!ucb->quiet)
            uc_report();
        if (ucb->mode == 'r' && ucb->okpkts > 0) {
            ret = uc_send_bitmap();
            if (ret != 0) {
                UC_EXIT("uc_send_bitmap", 1);
            }
        }
    }
    return 0;
}

int uc_recvfrom_timeout(struct uc_time_data* tdata)
{
    struct sockaddr_in addr;
    socklen_t addrlen;
    int ret;
    time_t start = time(NULL);
    int nread;
    int gotpkt = 0;

    uc_assert(tdata != NULL);
    uc_assert(tdata->pkt != NULL);
    uc_assert(tdata->type != 0);

    while (!gotpkt) {
        if (time(NULL) > start + tdata->timeout) {
            if (!ucb->quiet) {
                uc_println("uc_recvfrom_timeout timeout");
            }
            return UC_ERR_TIMEOUT;
        }
        usleep(1);

        if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
            continue;
        if (nread < ucb->pktsz) {
            continue;
        }
        if (ucb->verbose) {
            uc_println("trying to receive %d, %d bytes ready", tdata->type, nread);
        }
        addrlen = sizeof(addr);
        ret = recvfrom(ucb->sock,
            (void*)tdata->pkt,
            ucb->pktsz,
            0,
            (struct sockaddr*)&addr,
            &addrlen);

        if (ret < 0 && errno != EINTR) {
            //if (errno == ECONNREFUSED)
            if (ucb->verbose) {
                uc_println("ret %d, errno %d, %s", ret, errno, strerror(errno));
            }
            break;
        }
        if (ret != ucb->pktsz) {
            if (ucb->verbose) {
                uc_println("ret %d, pktsz %d", ret, ucb->pktsz);
            }
            continue;
        }
        if (ucb->verbose) {
            uc_println("uc_recvfrom_timeout got data, type %d, %d bytes", tdata->pkt->type, ucb->pktsz);
        }
        if (tdata->pkt->type == tdata->type) {
            gotpkt++;
        }
    }
    if (addr.sin_addr.s_addr != 0 && addr.sin_port != 0) {
        memcpy(&ucb->saddr, &addr, addrlen);
    }
    if (gotpkt) {
        if (ucb->verbose) {
            uc_println("uc_recvfrom_timeout ok from %s", ntoa(addr));
        }
        return UC_RECV_OK;
    }
    return UC_ERR_DISCONNECT;
}

void* uc_wait_ack(void* arg)
{
    struct uc_time_data* tdata = (struct uc_time_data*)arg;
    int ret;

    tdata->result = UC_ACK_OK;
    if (ucb->verbose) {
        uc_println("uc_wait_ack starting.");
    }
    ret = uc_recvfrom_timeout(tdata);
    tdata->result = ret;

    if (ucb->verbose) {
        uc_println("uc_wait_ack finishing, result is %d", tdata->result);
    }
    pthread_exit(NULL);
    return NULL;
}

int uc_send_recv(struct uc_time_data* tdata)
{
    pthread_t tid;

    if (tdata->flags & UC_ACK_WAIT_FLAG) {
        pthread_create(&tid, NULL, uc_wait_ack, (void*)tdata);
    }

    for (int delay = 0; delay < tdata->maxdelay; delay++) {
        if (ucb->verbose) {
            uc_println("sending %d, len %d, target %s", tdata->type, ucb->pktsz, ntoa(ucb->saddr));
        }
        int ret = sendto(ucb->sock,
            (void*)tdata->pkt,
            ucb->pktsz,
            0,
            (struct sockaddr*)&ucb->saddr,
            sizeof(ucb->saddr));

        if (ret <= 0) {
            if (ucb->verbose) {
                uc_println("sendrecv errno %d, %s", errno, strerror(errno));
            }
            return errno;
        }
        if (ret != ucb->pktsz)
            return errno;
        if (ucb->verbose) {
            uc_println("sent %d, len %d", tdata->type, ret);
        }
        usleep(delay);
    }

    pthread_join(tid, NULL);

    if (ucb->verbose)
        uc_println("uc_send_recv, result is %d", tdata->result);
    return tdata->result;
}

void uc_write_file(unsigned int len, void* buf)
{
    int ret;

    ret = write(ucb->fd, buf, len);
    if (ret < 0) {
        uc_println("write error, size %d, ret %d", len, ret);
        UC_FATAL("uc_write_file");
    }

    ucb->xferbytes += ret;
}

void* uc_file_writer(void* param)
{
    struct uc_buf* buf = NULL;
    struct uc_pkt_hdr* pkt = NULL;
    unsigned int seq = 0;
    struct uc_buf** wcache = NULL;
    struct uc_write_cache* wc = NULL;

    ucb->wchunk = 0;
    while ((wc = uc_pop_write_cache_list()) == NULL) {
        if (ucb->verbose)
            uc_println("null cache list wait...");
        usleep(100000);
    }
    wcache = wc->wcache;
    for (;;) {
        buf = uc_get_write_cache(seq, wcache, ucb->wchunk);
        if (buf == NULL) {
            usleep(1);
            continue;
        }
        pkt = buf->pkt;
        if (seq != pkt->pktnum) {
            uc_println(
                "filewriter: wchunk %d, seq %d != pktnum %d",
                ucb->wchunk,
                seq,
                pkt->pktnum);
            UC_FATAL("uc_file_writer seq mismatch");
        }

        if (ucb->verbose)
            uc_println("writing to file pkt seq %d sz %d", pkt->pktnum, ntohs(pkt->len));

        uc_write_file(ntohs(pkt->len), (void*)((char*)pkt + sizeof(struct uc_pkt_hdr)));
        if (ucb->lastseq == seq) {
            if (ucb->verbose)
                uc_println("written last pkt to file, seq %d", ucb->lastseq);
            break;
        }
        uc_free_buf(buf, wcache, ucb->wchunk);

        if ((++seq % ucb->chunksz) == 0) {
            if (ucb->verbose)
                uc_println("free wc chunk %d", ucb->wchunk);

            uc_free_write_cache(wc);

            while ((wc = uc_pop_write_cache_list()) == NULL) {
                if (ucb->verbose)
                    uc_println("null cache list wait...");
                usleep(100000);
            }
            wcache = wc->wcache;
            ucb->wchunk++;
        }
    }
    if (ucb->verbose)
        uc_println("Exiting writer");

    return 0;
}

void uc_recv_file()
{
    unsigned int delay = 0, nread = 0, seq = 0, chunk = 0, chunklim = 0;
    long sz = 0;
    int origplsz;
    struct uc_time_data tdata;
    struct uc_init_pl* initpl = NULL;
    struct uc_pkt_hdr *pkt = NULL, *ipkt = NULL;
    ssize_t ret = -1;
    socklen_t saddrlen = 0;
    struct uc_buf* buf = NULL;
    time_t tm;
    char* ts;
    struct uc_write_cache* wc;
    struct uc_buf** wcache;

    origplsz = ucb->plsz;
    ucb->sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (ucb->sock < 0)
        UC_FATAL("socket");

    ucb->saddr.sin_family = AF_INET;
    ucb->saddr.sin_port = htons(ucb->port);
    ucb->saddr.sin_addr.s_addr = INADDR_ANY;

    if (uc_set_sock_bufs() < 0)
        UC_FATAL("uc_set_sock_bufs");
    if (bind(ucb->sock, (struct sockaddr*)&ucb->saddr, sizeof(ucb->saddr)) < 0)
        UC_FATAL("bind");

    pkt = (struct uc_pkt_hdr*)uc_alloc((size_t)(sizeof(struct uc_pkt_hdr) + ucb->plsz));
    initpl = (struct uc_init_pl*)((char*)pkt + sizeof(struct uc_pkt_hdr));

    tdata.type = UC_INIT;
    tdata.pkt = pkt;
    tdata.timeout = 100;
    tdata.maxdelay = 20;
    ret = uc_recvfrom_timeout(&tdata);
    if (ret != UC_RECV_OK) {
        if (ucb->verbose)
            uc_println("failed to get pkt type %d", tdata.type);
        UC_FATAL("cannot get initial pkt");
    }

    ucb->plsz = ntohl(initpl->hdr.plsz);
    ucb->chunksz = ntohl(initpl->hdr.chunksz);

    if (ucb->filename == NULL) {
        ucb->filenamesz = ntohl(initpl->hdr.filenamesz);
        ucb->filename = (char*)uc_alloc((size_t)(ucb->filenamesz + 1));
        strncpy(ucb->filename, initpl->filename, ucb->filenamesz);
        ucb->filename[ucb->filenamesz] = '\0';
    }
    sz = pathconf(".", _PC_PATH_MAX);
    ucb->workdir = uc_alloc((size_t)sz);

    if (getcwd(ucb->workdir, (size_t)sz) == NULL)
        UC_FATAL("getcwd");

    if (!ucb->quiet)
        uc_print_info();

    tdata.timeout = 20;
    tdata.maxdelay = 10;
    tdata.flags = UC_ACK_FLAG | UC_ACK_WAIT_FLAG;
    tdata.type = UC_INIT;
    tdata.pkt = pkt;
    pkt->type = UC_INIT;
    pkt->flags = UC_ACK_FLAG;
    ret = uc_send_recv(&tdata);
    if (ret != UC_ACK_OK && ret != UC_RECV_OK)
        UC_EXIT("uc_send_recv", 2);

    if (ucb->plsz != origplsz) {
        uc_free(pkt);
        pkt = (struct uc_pkt_hdr*)uc_alloc((size_t)(sizeof(struct uc_pkt_hdr) + ucb->plsz));
    }
    if (ucb->verbose) {
        uc_println("connect to remote addr %s", ntoa(ucb->saddr));
    }
    if (connect(ucb->sock, (struct sockaddr*)&ucb->saddr, sizeof(ucb->saddr)) < 0)
        UC_FATAL("connect");

    ucb->fd = uc_create_file();
    uc_alloc_bitmap();
    uc_alloc_pool();
    gettimeofday(&ucb->starttime, 0);

    ucb->curchunk = chunk = 0;
    chunklim = (ucb->curchunk + 1) * ucb->chunksz;

    wc = uc_alloc_write_cache();
    uc_push_write_cache_list(wc);
    wcache = wc->wcache;

    pthread_create(&ucb->filewriter, &ucb->attr, uc_file_writer, NULL);

    while (!ucb->finished) {
        if (buf == NULL)
            buf = uc_get_buf();

        ipkt = buf->pkt; //XXX
        ret = recv(ucb->sock, (void*)ipkt, ucb->pktsz, 0);

        if (ret < 0 && errno != EINTR)
            UC_FATAL("recv");
        if (ret < nread)
            UC_FATAL("recv");

        switch (ipkt->type) {
        case UC_BITMAP:
            if (ucb->verbose)
                uc_println("bitmap request");
            ret = uc_send_bitmap();
            if (ret != 0) {
                UC_EXIT("uc_send_bitmap", 3);
            }
            break;
        case UC_FINISHED:
            ucb->finished++;
            if (ucb->verbose)
                uc_println("finished msg received");
            break;
        case UC_DATA:
            ucb->totalpkts++;
            ucb->totalbytes += ret;
            seq = ucb->seq = ipkt->pktnum = ntohl(ipkt->pktnum);
            if (ipkt->flags & UC_LAST_FLAG) {
                ucb->lastseq = seq;
                chunklim = seq + 1;

                if (ucb->verbose)
                    uc_println(
                        "received lastseq %d chunklim %d", ucb->lastseq, chunklim);
            }
            if (seq < (ucb->curchunk * ucb->chunksz) || seq >= chunklim) {
                if (ucb->verbose)
                    uc_println("chunk %d range (%d ~ %d) bad seq %d",
                        chunk,
                        chunk * ucb->chunksz,
                        chunklim,
                        seq);

                ucb->badpkts++;
                break;
            }
            if (uc_set_bit(seq)) {
                ++ucb->duplicates;

                if (ucb->verbose)
                    uc_println("duplicate seq %d", seq);
                break;
            }
            if (ucb->verbose)
                uc_println("ok seq %d", seq);

            ucb->okbytes += ntohs(ipkt->len);
            uc_insert_write_cache(buf, ipkt->pktnum, wcache, chunk);

            if ((++ucb->okpkts % ucb->chunksz) == 0) {
                if (ucb->verbose)
                    uc_println("chunk %d -> %d", chunk, chunk + 1);

                ret = uc_send_bitmap();
                if (ret != 0)
                    UC_EXIT("uc_send_bitmap", 4);

                ucb->curchunk = ++chunk;
                wc = uc_alloc_write_cache();

                uc_push_write_cache_list(wc);

                wcache = wc->wcache;
                uc_clear_bits();

                chunklim = (ucb->curchunk + 1) * ucb->chunksz;
            }
            if ((ucb->okpkts % 100) == 0) {
                ret = uc_send_bitmap(); /* XXX: */
                if (ret != 0) {
                    UC_EXIT("uc_send_bitmap", 5);
                }
            }

            if (ucb->okpkts == ucb->lastseq + 1) {
                if (ucb->verbose)
                    uc_println("finished, okpkts %d", ucb->okpkts);

                ucb->finished++;
            }
            buf = NULL;
            break;
        default:
            break;
        }
    }
    if (ucb->verbose)
        uc_println("finished, out of main recv loop");

    ucb->finished++;
    pkt->type = UC_FINISHED;
    pkt->pktnum = htonl(ucb->okpkts);

    if (ucb->verbose)
        uc_println("sending FINISHED msg");

    tdata.maxdelay = 10;
    tdata.flags = UC_ACK_FLAG | UC_ACK_WAIT_FLAG;
    tdata.type = UC_FINISHED;
    pkt->flags = UC_ACK_FLAG;
    tdata.pkt = pkt;
    tdata.timeout = 20;
    ret = uc_send_recv(&tdata);
    if (ret != UC_ACK_OK && ret != UC_RECV_OK)
        uc_println("uc_send_recv, ret %d", ret);

    pthread_join(ucb->filewriter, NULL);
    time(&tm);
    ts = ctime(&tm);
    close(ucb->fd);
    if (ucb->verbose) {
        uc_println("closing file %s, %s", ucb->filename, ts);
        uc_println("Exiting receiver");
    }
}

int uc_send_block(unsigned int seq)
{
    int ret = -1;
    struct timeval tv;
    unsigned int diff = 0;
    struct uc_pkt_hdr* pkt = NULL;

    ucb->seq = seq;
    pkt = uc_read_file(ucb->seq);

    //uc_delay_usec(ucb->ipg + 10 * ucb->iter);
    uc_delay_usec(ucb->ipg);

    //if (ucb->verbose && (pkt->flags & UC_LAST_FLAG))
    //    uc_println("sending LAST msg seq %d", ntohl(pkt->pktnum));

    ret = send(ucb->sock, (void*)pkt, ucb->pktsz, 0);

    if (errno == ECONNREFUSED) {
        if (ucb->verbose) {
            uc_println("uc_send_block  ECONNREFUSED");
        }
        return errno;
    }
    if (ret <= 0) {
        if (ret != ENOBUFS)
            return 0;
        UC_FATAL("send");
    }
    if (ret != ucb->pktsz)
        UC_FATAL("send");

    ucb->totalbytes += ret;
    ucb->totalpkts++;

    gettimeofday(&tv, 0);
    diff = uc_usec_diff(&ucb->starttime, &tv);
    ucb->rate = (unsigned int)((float)ucb->totalbytes * 1000000 / diff);

    if (ucb->ipgfixed)
        return 0;
    if (ucb->desiredrate == 0)
        return 0;

    if (ucb->rate > ucb->desiredrate)
        ucb->ipg++;
    else if (ucb->ipg > 0)
        ucb->ipg--;
    return 0;
}

void uc_xget_input(struct uc_pkt_hdr* ipkt, int nread)
{
    int ret = -1;
    unsigned int len = 0, acked = 0;
    struct iovec iov[2];
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    ret = recv(ucb->sock, (void*)ipkt, sizeof(struct uc_pkt_hdr), MSG_PEEK);

    if (ret < 0 && errno != EINTR)
        UC_FATAL("recv");
    if (ret < sizeof(struct uc_pkt_hdr))
        UC_FATAL("recv");
    switch (ipkt->type) {
    case UC_FINISHED:
        ret = recv(ucb->sock, (void*)ipkt, nread, 0);

        if (ret < 0 && errno != EINTR)
            UC_FATAL("recv");
        if (ucb->verbose)
            uc_println("finished, received FINISHED msg.");

        ucb->finished++;
        ucb->acked = ntohl(ipkt->pktnum);
        break;
    case UC_BITMAP:
        len = ntohs(ipkt->len);
        acked = ntohl(ipkt->pktnum);

        if (acked <= ucb->acked) {
            ret = recv(ucb->sock, (void*)ipkt, nread, 0);
            if (ret < 0 && errno != EINTR)
                UC_FATAL("recv");

            ipkt->type = 0;
            break;
        }
        ucb->acked = acked;
        iov[0].iov_base = (void*)ipkt;
        iov[0].iov_len = sizeof(struct uc_pkt_hdr);
        iov[1].iov_base = (void*)ucb->bitmap;
        iov[1].iov_len = len;
        ret = readv(ucb->sock, iov, 2);

        if (ret <= 0)
            UC_FATAL("readv");
        if (ret < len + sizeof(struct uc_pkt_hdr))
            UC_FATAL("readv");
        break;
    case UC_INIT:
        if (ucb->verbose)
            uc_println("unexpected %d", ipkt->type);

        ret = recv(ucb->sock, (void*)ipkt, ucb->pktsz, 0);

        if (ret < 0 && errno != EINTR)
            UC_FATAL("recv");
        if (ret < ucb->pktsz) {
            UC_FATAL("recv");
        }
        break;
    default:
        ret = recvfrom(ucb->sock, (void*)ipkt, nread, 0, (struct sockaddr*)&addr, &addrlen);
        if (ret < 0) {
            if (errno == EINTR)
                break;
            UC_FATAL("recv");
        }
        if (ret < nread) {
            uc_println("ret %d < nread %d, ipkttype %d, from %s",
                ret,
                nread,
                ipkt->type,
                ntoa(addr));
        }
        break;
    }
}

unsigned int
uc_adv_seq(unsigned int seq, unsigned int limit)
{
    uc_assert(seq <= limit);
    if (seq == ucb->lastseq || ++seq == limit) {
        seq = ucb->curchunk * ucb->chunksz;
        ucb->iter++;
    }
    return seq;
}

void uc_xmit_file()
{
    int ix = 0, nread = 0, chunkdone = 0;
    unsigned int flen = 0, delay = 0, seq = 0, chunk = 0, chunklim = 0;
    struct uc_pkt_hdr *pkt = NULL, *ipkt = NULL;
    struct sockaddr_in rsaddr;
    ssize_t ret = -1;
    struct uc_init_pl* initpl = NULL;
    socklen_t saddrlen = 0;
    long sz = 0;

    ucb->fd = uc_open_file();
    ucb->sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (ucb->sock < 0)
        UC_FATAL("socket");

    ucb->saddr.sin_family = AF_INET;
    ucb->saddr.sin_port = htons(ucb->port);
    ucb->saddr.sin_addr.s_addr = inet_addr(ucb->host);
    memcpy((void*)&rsaddr, (void*)&ucb->saddr, sizeof(ucb->saddr));

    if (uc_set_sock_bufs() < 0)
        UC_FATAL("uc_set_sock_bufs");

    pkt = (struct uc_pkt_hdr*)uc_alloc((size_t)(sizeof(struct uc_pkt_hdr) + ucb->plsz));
    ipkt = (struct uc_pkt_hdr*)uc_alloc((size_t)(sizeof(struct uc_pkt_hdr) + ucb->plsz));
    initpl = (struct uc_init_pl*)((char*)pkt + sizeof(struct uc_pkt_hdr));

    pkt->type = UC_INIT;

    initpl->hdr.plsz = htonl(ucb->plsz);
    initpl->hdr.chunksz = htonl(ucb->chunksz);

    flen = strlen(ucb->filename);
    initpl->hdr.filenamesz = htonl(flen);
    strncpy(initpl->filename, ucb->filename, flen);
    initpl->filename[flen] = '\0';

    sz = pathconf(".", _PC_PATH_MAX);
    ucb->workdir = uc_alloc((size_t)sz);

    if (getcwd(ucb->workdir, (size_t)sz) == NULL)
        UC_FATAL("getcwd");

    struct uc_time_data tdata;
    tdata.timeout = 10;
    tdata.flags = UC_ACK_FLAG | UC_ACK_WAIT_FLAG;
    tdata.type = UC_INIT;
    pkt->flags = UC_ACK_FLAG;
    tdata.pkt = pkt;
    tdata.maxdelay = 20;
    ret = uc_send_recv(&tdata);
    if (ret != UC_ACK_OK && ret != UC_RECV_OK)
        UC_EXIT("uc_send_recv", 6);
    if (connect(ucb->sock, (struct sockaddr*)&ucb->saddr, sizeof(ucb->saddr)) < 0)
        UC_FATAL("connect");

    if (ucb->verbose)
        uc_println("connected");
    if (!ucb->quiet)
        uc_print_info();

    uc_alloc_bitmap();
    uc_alloc_pool();
    uc_alloc_read_cache();
    gettimeofday(&ucb->starttime, 0);

    ucb->reporttime = ucb->starttime;
    chunk = 0;

    while (!ucb->finished) {
        seq = chunk * ucb->chunksz;
        if (ucb->verbose)
            uc_println("start sending chunk %d, range %d ~ %d",
                chunk,
                seq,
                (chunk + 1) * ucb->chunksz);

        chunkdone = 0;
        ucb->iter = 0;
        chunklim = seq + ucb->chunksz;

        while (!chunkdone) {
            nread = 0;
            if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
                UC_FATAL("fionread");

            while (nread >= sizeof(struct uc_pkt_hdr)) {
                uc_xget_input(ipkt, nread);
                if (ipkt->type == UC_FINISHED) {
                    ucb->finished++;
                    ucb->acked = ntohs(ipkt->pktnum);
                    if (ucb->verbose)
                        uc_println("finished, received FINISHED");

                    break;
                }
                if (ipkt->type == UC_BITMAP) {
                    if (ntohl(ipkt->pktnum) == (chunk + 1) * ucb->chunksz) {
                        if (ucb->verbose)
                            uc_println("chunk %d done", chunk);

                        chunkdone++;
                        break;
                    }
                }
                nread = 0;
                if (ioctl(ucb->sock, FIONREAD, &nread) < 0)
                    UC_FATAL("fionread");
            }
            if (ucb->finished) {
                chunkdone++;
            }
            uc_assert(seq >= chunk * ucb->chunksz);
            uc_assert(seq < (chunk + 1) * ucb->chunksz);
            if (ucb->iter > 0 && uc_test_bit(seq)) {
                if (seq == ucb->lastseq) {
                    uc_println("last seq %d acked, break", ucb->lastseq);
                    ucb->finished++;
                    break;
                }
                seq = uc_adv_seq(seq, chunklim);
                if (ucb->verbose) {
                    uc_println("iter %d seq %d curchunk %d chunksz %d chunklim %d continue",
                        ucb->iter, seq, ucb->curchunk, ucb->chunksz, chunklim);
                }
                continue;
            }
            if (ucb->verbose)
                uc_println("sending seq %d", seq);

            ret = uc_send_block(seq);
            if (ret != 0) {
                if (ret == ECONNREFUSED)
                    ucb->finished++;
                else
                    UC_EXIT("uc_send_block", 7);
            }
            seq = uc_adv_seq(seq, chunklim);

            if (ucb->verbose)
                uc_println("next seq %d", seq);
        }
        chunkdone = 0;
        ucb->curchunk = ++chunk;

        if (ucb->verbose)
            uc_println("chunk %d -> %d", chunk - 1, chunk);

        uc_recycle_read_cache();
        uc_clear_bits();
    }
    uc_free(pkt);
    uc_free(ipkt);

    if (ucb->verbose)
        uc_println("Exiting transmitter.");
}

#define PERM_READ (1 << 0)
#define PERM_WRITE (1 << 1)
#define PERM_EXEC (1 << 2)
static const char* const usage[] = {
    "ucat [options] [[--] args]",
    "ucat [options]",
    NULL,
};

int uc_main(int argc, const char** argv)
{
    unsigned int tdiff = 0, tmin = 0, tsec = 0;
    time_t tm;
    ucb = (struct ucb*)uc_alloc(sizeof(struct ucb));

    ucb->lastseq = 0xffffffff;
    ucb->mode = 'r';
    ucb->port = UC_PORT;
    ucb->mtu = UC_MTU;
    ucb->filename = NULL;
    ucb->sockbufsz = 1024 * 512;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Basic options"),
        OPT_BOOLEAN('v', "verbose", &ucb->verbose, "verbose"),
        OPT_BOOLEAN('q', "quiet", &ucb->quiet, "quiet"),
        OPT_BOOLEAN('d', "debug", &ucb->debug, "debug"),
        OPT_STRING('t', "target", &ucb->host, "target host name or address. If -t is used, sending is implied."),
        OPT_STRING('f', "filename", &ucb->filename, "filename"),
        OPT_INTEGER('p', "port", &ucb->port, "port num"),
        OPT_INTEGER('m', "mtu", &ucb->mtu, "mtu"),
        OPT_INTEGER('s', "sockbufsz", &ucb->sockbufsz, "socket buffer size"),
        OPT_INTEGER('i', "interval", &ucb->ipg, "interval between packets"),
        OPT_INTEGER('r', "rate", &ucb->desiredrate, "rate"),
        OPT_GROUP("Bits options"),
        OPT_BIT(0, "read", &ucb->perms, "read perm", NULL, PERM_READ, OPT_NONEG),
        OPT_BIT(0, "write", &ucb->perms, "write perm", NULL, PERM_WRITE),
        OPT_BIT(0, "exec", &ucb->perms, "exec perm", NULL, PERM_EXEC),
        OPT_END(),
    };
    char* ts = NULL;

    struct argparse argparse;
    argparse_init(&argparse, options, usage, 0);
    argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
    argc = argparse_parse(&argparse, argc, argv);

    if (ucb->verbose) {
        uc_println("filename %s host %s", ucb->filename, ucb->host);
    }
    if (ucb->filename == NULL)
        ucb->filename = "testfile";
    if (ucb->host == NULL)
        ucb->mode = 'r';
    else
        ucb->mode = 't';

    pthread_attr_init(&ucb->attr);
    pthread_create(&ucb->timer, &ucb->attr, uc_timer, NULL);

    if (ucb->plsz == 0)
        ucb->plsz = ucb->mtu - 20 - 8 - sizeof(struct uc_pkt_hdr);

    ucb->chunksz = ucb->plsz << 3;
    ucb->pktsz = ucb->plsz + sizeof(struct uc_pkt_hdr);
    if (ucb->verbose) {
        uc_println("pktsz %d", ucb->pktsz);
    }
    ucb->poolsz = ucb->chunksz;

    if (!ucb->quiet) {
        time(&tm);
        ts = ctime(&tm);
        uc_println("%s version %s , start time: %s process id %d",
            UC_PROG_NAME,
            UC_VERSION,
            ts,
            getpid());
    }
    if (ucb->mode == 't') {
        if (!ucb->quiet)
            uc_println("Sending...");

        uc_xmit_file();
    } else {
        if (!ucb->quiet)
            uc_println("Receiving...");

        pthread_mutex_init(&ucb->bufmutex, NULL);
        pthread_mutex_init(&ucb->cachemutex, NULL);

        uc_recv_file();
    }
    gettimeofday(&ucb->endtime, 0);
    tdiff = uc_usec_diff(&ucb->starttime, &ucb->endtime);

    tmin = tdiff / 1000000 / 60;
    tsec = tdiff / 1000000 % 60;

    if (!ucb->quiet) {
        unsigned long long val;
        time(&tm);
        ts = ctime(&tm);

        uc_println("%lld bytes transferred", ucb->xferbytes);
        uc_println("end time: %s", ts, getpid());
        uc_println("Exiting, elapsed time: %.2d min %.2d secs", tmin, tsec);

        //val = ucb->seq;
        //val = val * ucb->plsz;
        //uc_println("%lld bytes per second", val / (tdiff / 1000000));
    }
    fflush(stdout);
    fflush(stderr);
    return 0;
}

#ifndef __COMPILE_LIBRARY__
int main(int argc, char* argv[])
{
    return uc_main(argc, (const char**)argv);
}
#endif
