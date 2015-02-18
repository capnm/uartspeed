//
//	Copyright (c) 2015 Martin Capitanio <capnm@capitanio.org>
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.
//
// Allows to set almost arbitrary speed for an UART on Linux.
//
// See:
// 	http://stackoverflow.com/questions/12646324/how-to-set-a-custom-baud-rate-on-linux
// 	https://gist.github.com/sentinelt/3f1a984533556cf890d9
//
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <asm/termios.h>

extern int ioctl(int a, unsigned int b, void *tio);
static void setraw(struct termios2* t2);

int main(int argc, char* argv[])
{

	if (argc < 3) {
		printf("%s device speed\n\nSet speed for a serial device.\n"
			"For instance:\n"
			"    %s /dev/ttyUSB0 38400 [raw]\n\n", argv[0], argv[0]);
		return -1;
	}

	int fd = open(argv[1], O_RDONLY);
	int speed = atoi(argv[2]);

	struct termios2 tio;
	ioctl(fd, TCGETS2, &tio);

	printf("speed (i/o): %d %d\n", tio.c_ispeed, tio.c_ospeed);
	if (argc == 4 && argv[3][0] == 'r') {
		puts("set raw mode");
		setraw(&tio);
	}

	if ((tio.c_ispeed != speed) || (tio.c_ispeed != speed)) {
		printf("set speed to: %d %d\n", speed, speed);
	}

	tio.c_cflag &= ~CBAUD;
	tio.c_cflag |= BOTHER;
	tio.c_ispeed = speed;
	tio.c_ospeed = speed;

	int r = ioctl(fd, TCSETS2, &tio);
	if (r != 0) {
		puts("Error: ioctl:");
		perror("ioctl");
		close(fd);
		return 1;
	}

	ioctl(fd, TCGETS2, &tio);
	close(fd);
	return 0;
}

// Disable as much crap as possible.
static void setraw(struct termios2* t2)
{
	// input mode flags
	t2->c_iflag &= ~(IGNBRK|BRKINT|IGNPAR|PARMRK|INPCK|ISTRIP|
		INLCR|IGNCR|ICRNL|IXON|IXOFF);
	// output mode flags
	t2->c_oflag &= ~OPOST;
	// local mode flags
	t2->c_lflag &= ~(ISIG|ICANON|ECHO|ECHONL|IEXTEN);
	// control mode flags
	t2->c_cflag &= ~(CSIZE|PARENB);
	t2->c_cflag |= CS8;
	// control characters - wait for 1 byte
	t2->c_cc[VTIME] = 0;
	t2->c_cc[VMIN] = 1;
}
