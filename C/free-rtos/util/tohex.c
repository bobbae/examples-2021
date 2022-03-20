/* $Revision: 1.2 $ */

/*
 * Tohex  is a utility for converting UNIX a.out files into various readable
 * hex  formats  suitable for downloading or inter-system file  transfer.
 * Motorola S-records, Tek-hex, and Intel Hex are currently  supported. Other
 * formats  can  be  added easily by adding  a  formatter  module  and
 * modifying  getargs.   Toshex, toihex,  and  totekhex are the current
 * formatter modules.  Tohex does not support System V COFF object files.
 * 
 * (c) Copyright 1984, 1987 by Phillip L. Harbison
 * 
 * Permission is granted to use, reproduce and distribute this pro- gram  by any
 * means, as long as no fee is charged, and provided this notice is always
 * included in any copies.
 * 
 * The   author   hereby   dislaims  any  and  all  guarantees  and warranties,
 * both  expressed and implied; and no fitness for any purpose  whatsoever is
 * claimed or implied.  The author will not be  responsible  for  any damages
 * resulting from the use of this software.   Tohex is provided as is, with
 * no promise of support, but  I  will try to respond to polite bug reports
 * or suggestions for  improvements  that  are  forwarded  to  me via  any of
 * the following means.
 * 
 * Live: Phil Harbison Mail: 3409 Grassfort Drive, Huntsville, AL 35805-5421
 * UUCP: uunet!madhat!alvitar or alvitar@madhat.uu.net BELL: 205-881-4317,
 * 205-535-2500
 */

/*
 * hjb@netcom.com: tohex is now modified to handle different byte order
 * architectures: namely intel 80x86 and mc680x0. the relevant option is:  -w
 * (swaps bytes in a.out header)
 */

/*
 * $Revision: 1.2 $ $Id: tohex.c,v 1.2 1995/08/17 00:59:21 hjb Exp $
 * $Log: tohex.c,v $
 * Revision 1.2  1995/08/17  00:59:21  hjb
 * indent changes -> to BSD kernel style
 * Revision 1.1.1.1  1995/08/13  20:01:43  hjb FREE_0_2
 */

#include <stdio.h>
#include <a.out.h>

#define MAXSIZE 256
#define NULL 0
#define TRUE 1
#define FALSE 0
#define INTEL	'i'
#define LENGTH	'l'
#define SWAP	'S'
#define SWAP_HEADER 'w'
#define MOT	's'
#define TEKHEX	't'

char            digits[] = "0123456789ABCDEF";	/* for use in output
						 * conversion */
char            record[MAXSIZE + 1];	/* where we put binary input data */
int             (*hexout) ();	/* pointer to conversion function */
int             offset;		/* starting address */
int             oflag;		/* starting address specified as arg */
int             recsize;	/* maximum data bytes/output record */
int             chksum;		/* output record checksum */
int             toshex(), toihex(), totekhex();	/* conversion functions */
struct exec     header;		/* where we put the a.out header */
FILE           *infile;		/* input file descriptor */

int             swapinput = 0;

#ifdef i386			/* little endian */
int             swapheader = 1;
#else
int             swapheader = 0;
#endif

FILE           *swap_file();

main(argc, argv)
	int             argc;
	char           *argv[];
{
	int             addr, maxaddr, count;

	/* process the command line arguments */
	if (!getargs(argc, argv)) {
		usage();
		exit(1);
	}
	if (swapinput) {
		infile = swap_file(infile);
		if (infile == 0) {
			fprintf(stderr, "ERROR: input file swapping failed\n");
			exit(2);
		}
	}
	/* read the a.out file header */
	if (!fread(&header, sizeof(struct exec), 1, infile)) {
		fprintf(stderr, "tohex: unable to read file header\n");
	}
	if (swapheader)
		swap_header(&header);
	if (N_BADMAG(header)) {
		fprintf(stderr, "tohex: not an a.out file\n");
		exit(1);
	}
	if (header.a_trsize || header.a_drsize) {
		fprintf(stderr, "tohex: input file not stripped\n");
		exit(1);
	}
	/* print interesting trivia about the file */
	fprintf(stderr, "entry point: %08X\n", header.a_entry);
	fprintf(stderr, "text size: %06X (%d)\n", header.a_text,
		header.a_text);
	fprintf(stderr, "data size: %06X (%d)\n", header.a_data,
		header.a_data);

	/*
	 * if no starting address was specified use the entry point specified
	 * in the header.  This is usually appropriate for a.out files.
	 */
	if (!oflag)
		offset = header.a_entry;

	/* calculate size of input */
	maxaddr = header.a_text + header.a_data;

	/* loop until out of input */
	for (addr = 0; addr <= maxaddr;) {
		/*
		 * try to read count bytes from infile into record where
		 * count is the lesser of recsize or the number of bytes
		 * remaining in the object image.
		 */
		count = ((addr + recsize) <= maxaddr) ?
			recsize : (maxaddr - addr);
		count = fread(record, 1, count, infile);
		/* are we at EOF ? */
		if (!count) {
			break;
		}
		/*
		 * convert record from binary to hex format.  hexout points
		 * to the specified conversion function.
		 */
		(*hexout) (count, addr, record);
		addr += count;
	}
	/* force the termination record by requesting a zero-length packet. */
	(*hexout) (0, 0, record);
	fclose(infile);
	exit(0);
}

/*
 * getargs processes the command line arguments.  It expects to see hex
 * output type an input filename, and (optionally) a starting address offset
 * in any order.
 */
getargs(argc, argv)
	int             argc;
	char           *argv[];
{
	int             index, state, maxsize, tmp;

	state = TRUE;
	oflag = FALSE;
	hexout = NULL;
	infile = (FILE *) NULL;

	/* default record size is 32 */
	recsize = 32;

	/* process each argument */
	for (index = 1; index < argc; index++) {
		/* see if its a flag */
		if (*argv[index] == '-') {
			/* must be a hex output type */
			switch (*(argv[index] + 1)) {
			case MOT:
				/* motorola S-records */
				hexout = toshex;
				maxsize = 251;
				break;
			case INTEL:
				hexout = toihex;
				maxsize = 255;
				break;
			case TEKHEX:
				hexout = totekhex;
				maxsize = 255;
				break;
			case LENGTH:
				if (sscanf(argv[index] + 2, "%d", &tmp) == 1) {
					recsize = tmp;
				} else {
					badarg(argv[index]);
					state = FALSE;
				}
				break;
			case SWAP_HEADER:
				swapheader = 1;
				break;
			case SWAP:
				swapinput = 1;
				break;
			default:
				badarg(argv[index]);
				state = FALSE;
			}
		}
		/* is it an offset? */
		else if (*argv[index] == '+') {
			/* yes, must be hexadecimal */
			if (sscanf(argv[index] + 1, "%x", &offset) == 1) {
				oflag = TRUE;
			} else {
				badarg(argv[index]);
				state = FALSE;
			}
		}
		/* else it must be a filename */
		else if ((infile = fopen(argv[index], "r")) == NULL) {
			fprintf(stderr, "unable to open %s\n", argv[index]);
		}
	}
	/* was a format specified? */
	if (hexout) {
		/* does recsize exceed maxsize? */
		if (recsize > maxsize) {
			fprintf(stderr, "tohex: record size too large\n");
			state = FALSE;
		}
	} else {
		fprintf(stderr, "tohex: format not specified\n");
		state = FALSE;
	}
	/* do we have an input file? */
	if (!infile) {
		fprintf(stderr, "tohex: no input file\n");
		state = FALSE;
	}
	return (state);
}

/*
 * toshex performs the binary to Motorola S-record con- version function. The
 * argument "dptr" points to a string of bytes to be converted.  "count"
 * specifies the size of the string, while "addr" specifies the starting
 * address for this block of output.  Only S2 and S8 records are supported
 * (24-bit addresses).
 */
toshex(count, addr, dptr)
	int             count, addr;
	char           *dptr;
{
	/* adjust address for starting offset */
	addr += offset;
	/* and mask off all but 24 bits */
	addr &= 0xFFFFFF;
	/* no more bytes ? */
	if (!count) {
		/* build a termination (S8) record */
		sheader('8', 0, offset);
	} else {
		/* build a data (S2) record */
		sheader('2', count, addr);
		/* output data */
		for (; count--;) {
			hexpair(*dptr++);
		}
	}
	/* output chksum, Motorola uses 1s complement */
	hexpair(~chksum);
	putchar('\n');
}

/*
 * sheader: called by toshex to generate the header of an s-record.  Type is
 * either 2 or 8.
 */
sheader(type, count, addr)
	char            type;
	int             count, addr;
{
	putchar('S');
	putchar(type);
	chksum = 0;

	/* output record length */
	hexpair(count + 4);

	/* output address */
	hexpair(addr >> 16);
	hexpair(addr >> 8);
	hexpair(addr);
}

/*
 * toihex performs the binary to Intel-hex conversion function.  The argument
 * "dptr" points to a string of bytes to be converted.  "count" specifies the
 * size of the string, while "addr" specifies the starting address for this
 * block of output.
 */
toihex(count, addr, dptr)
	int             count, addr;
	char           *dptr;
{
	/* no more data? */
	if (!count) {
		/* output termination packet */
		printf(":0000000000\n");
	} else {
		/* Start of header character */
		putchar(':');
		/* record length */
		chksum = 0;
		hexpair(count);
		/* address */
		hexpair(addr >> 8);
		hexpair(addr);
		/* record type, always 0 */
		hexpair(0);
		/* data */
		for (; count--;) {
			hexpair(*dptr++);
		}
		/* checksum, intel uses 2s complement */
		hexpair(-chksum);
		putchar('\n');
	}
}

/*
 * totekhex performs the binary to Tek-hex conversion function.  The argument
 * "dptr" points to a string of bytes to be converted.  "count" specifies the
 * size of the string, while "addr" specifies the starting address for this
 * block of output.
 */
totekhex(count, addr, dptr)
	int             count, addr;
	char           *dptr;
{
	int             index;

	/* no more data? */
	if (!count) {
		/* output termination packet */
		printf("/00000000\n");
	} else {
		chksum = 0;
		/* Start of header character */
		putchar('/');
		/* address */
		tekpair(addr >> 8);
		tekpair(addr);
		tekpair(count);
		/* header checksum */
		tekpair(chksum);
		chksum = 0;
		/* data */
		for (; count--;) {
			tekpair(*dptr++);
		}
		/* data checksum */
		tekpair(chksum);
		putchar('\n');
	}
}

/*
 * hexpair outputs the two character hex-ascii equivalent of the least
 * significant eight bits of its argument. It also performs chksum
 * accumulation.  This is a common support routine for the toshex and toihex
 * functions.
 */
hexpair(arg)
	int             arg;
{
	putchar(digits[(arg >> 4) & 0x0F]);
	putchar(digits[arg & 0x0F]);
	chksum += arg;
}

/*
 * tekpair: called by totekhex to convert a byte to a hex-ascii pair.  Tekhex
 * computes the checksum using nibbles rather than bytes.
 */
tekpair(arg)
	int             arg;
{
	int             lsn, msn;

	msn = (arg >> 4) & 0x0F;
	lsn = arg & 0x0F;
	putchar(digits[msn]);
	putchar(digits[lsn]);
	chksum = chksum + lsn + msn;
}

/* usage prints the usage message, a standard part of most error messages. */
usage()
{
	fprintf(stderr,
	    "usage: tohex -{s,i} [-S] [-l<length>] [+<offset>] filename\n");
	fprintf(stderr,
		"\t-s: motorola Srec\n\t-i: intel hex\n\t-w: swap bytes in"
		" the a.out header\n\t-S: swap all bytes in the input"
		"file.\n");
}

/*
 * badarg prints the rejected argument pointed to by str, prepended by an
 * error message.
 */
badarg(str)
	char           *str;
{
	fprintf(stderr, "tohex: invalid argument %s\n", str);
}

FILE           *
swap_file(infile)
	FILE           *infile;
{
	char            buf[5];
	int             j, i;
	FILE           *tmp;

	tmp = tmpfile();
	if (tmp == 0) {
		printf("ERROR: can't create a tmp file <%s>.\n");
		return 0;
	}
	while ((i = fread(buf, 1, 4, infile)) != -1) {
		if (i == 0)
			break;
		--i;
		for (j = 0; j < 4 && (i - j) >= 0; j++) {
			fputc(buf[i - j], tmp);
		}
	}
	fflush(tmp);
	rewind(tmp);
	fclose(infile);
	return (tmp);
}

swap_header(header)
	struct exec    *header;
{
	int             i, j;
	char           *cp = (char *) header;
	struct exec     new;
	char           *np = (char *) &new;

	for (i = 0, cp = cp + 3; i < sizeof(*header) / 4; i++, cp = cp + 8) {
		for (j = 0; j < 4; j++)
			*np++ = *cp--;
	}
#ifdef DEBUG
	printf("old----------------------\n");
	dump(header, sizeof(*header));
	printf("new----------------------\n");
	dump(&new, sizeof(new));
#endif
	*header = new;
}

#ifdef DEBUG
typedef unsigned char u_char;
typedef unsigned long u_long;

/* table of  printable ASCII characters */
u_char         *ascii_table = (u_char *) " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~.";

dump(addr, len)
{
	int             i, j;
	u_char          buf[33];/* 32 chars plus NUL */
	u_char         *bp = buf;
	u_char         *ap;
	u_long          val;

	/* assume 4 bytes alignment */
	for (i = 0; i < len; i += 4, addr += 4) {
		if ((i % 0x10) == 0) {
			if (i != 0) {
				printf("    %s\n", buf);
				bp = buf;
			}
			printf("  %8.0x:  ", addr);
		}
		ap = (u_char *) addr;
		val = (u_long) (((int) *ap) << 24 |
				((int) *(ap + 1)) << 16 |
				((int) *(ap + 2)) << 8 |
				((int) *(ap + 3)));

		printf("%8.0x ", val);

		for (ap = (u_char *) addr, j = 0; j < 4; j++, ap++, bp++) {
			if ((int) (*ap) < 128 && (int) (*ap) > 31)
				*bp = ascii_table[(int) (*ap) - 32];
			else
				*bp = '.';
		}
		*bp = '\0';
	}

	printf("    %s\n", buf);
}

#endif
