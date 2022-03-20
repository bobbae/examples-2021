/* $Revision: 1.2 $ */

/*
 * A BSD tip-like program to communicate with a target machine over serial
 * line.  Especially for host machines that do not have the tip program (e.g.
 * Linux).  cu, tip and kermit work well but this program is simpler, and can
 * be customized to do what you need to do easily.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/termio.h>
#include <sys/termios.h>
#include <sys/file.h>
#include <signal.h>

void            input(), output();
void            setup();
void            control_c();

struct termios  savetty, rawtty;
int             tty, pid;

main()
{
	if ((tty = open("/dev/ttyS1", O_RDWR | O_NDELAY)) < 0) {
		perror("open");
		exit(1);
	}
	printf("^D: to quit, ~C: to send S-record file\n");
	setup();
	if ((pid = fork()))
		input();
	else
		output();
}

void
cleanup()
{
	printf("\n");
	tcsetattr(tty, TCSANOW, &savetty);
	kill(pid, SIGTERM);
	printf("Exiting...\r\n");
	exit(1);
}

void
control_c()
{
	cleanup();
}

void
fill_tio(struct termios * tip)
{
	tip->c_lflag &= ~(ICANON | ISIG | ECHO | ECHONL | ECHOE | ECHOK);
	tip->c_cflag |= CLOCAL | HUPCL;
	tip->c_cflag = (tip->c_cflag & ~CBAUD) | B9600;
	tip->c_cflag = (tip->c_cflag & ~CSIZE) | CS8;
	tip->c_cflag |= CREAD | CRTSCTS;
	tip->c_iflag |= IXON | IXOFF | IXANY;
	tip->c_iflag |= (BRKINT | IGNPAR);
	tip->c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC |
			  INPCK | ISTRIP);
	tip->c_oflag &= ~OPOST;
	tip->c_oflag |= IXON | IXOFF | IXANY;
	tip->c_cc[VMIN] = 1;
	tip->c_cc[VTIME] = 5;
}

void
setup()
{
	struct termios  tio;
	int             mcs;

	tcgetattr(0, &savetty);
	signal(SIGINT, control_c);
	signal(SIGTERM, control_c);
	tio = savetty;
	fill_tio(&tio);
	tcsetattr(0, TCSANOW, &tio);
	tcgetattr(0, &rawtty);
	tcgetattr(0, &tio);
	fill_tio(&tio);
	tcsetattr(tty, TCSANOW, &tio);
	tcsetattr(0, TCSADRAIN, &tio);
	tcsetattr(tty, TCSADRAIN, &tio);
	ioctl(tty, TCFLSH, 2);
}

void
input()
{
	int             j;
	char            ch;

	for (;;) {
		j = read(tty, &ch, 1);
		if (j > 0)
			write(1, &ch, 1);
	}
}

void
sendchar(char ch)
{
	char            cr = '\r';

	if (ch == '\n')
		write(tty, &cr, 1);
	write(tty, &ch, 1);
}

void
sendstr(char *str)
{
	while (*str) {
		sendchar(*str);
		str++;
	}
}
void
backspace(void)
{
	char            erase = '';

	write(1, &erase, 1);
}

void
sendfile()
{
	char            file[256];
	FILE           *fp;
	int             x;
	char            spin[] = "/-\\|";

	tcsetattr(0, TCSANOW, &savetty);
	printf("File name to send > ");
	fflush(stdout);
	if (gets(file) == 0) {
		tcsetattr(0, TCSANOW, &rawtty);
		return;
	}
	fp = fopen(file, "r");
	if (fp == 0) {
		perror("fopen");
		fprintf(stderr, "ERROR: file <%s> does not exist?\n", file);
		tcsetattr(0, TCSANOW, &rawtty);
		return;
	}
	printf("Sending file <%s>\n", file);
	x = 0;
	while (fgets(file, sizeof(file), fp) != 0) {
		backspace();
		usleep(1400);
		sendstr(file);
		write(1, &spin[x], 1);
		x = (x + 1) & 3;
	}
	tcsetattr(0, TCSANOW, &rawtty);
}

void
output()
{
	int             j;
	char            ch;

	for (;;) {
		j = read(0, &ch, 1);
		if (j > 0) {
			if (ch == '')
				cleanup();
			if (ch == '~') {
				j = read(0, &ch, 1);
				if (j > 0) {
					switch (ch) {
					case 'C':
						sendfile();
						continue;
					case 'q':
						cleanup();
					}
				}
			}
			sendchar(ch);
		}
	}
}
