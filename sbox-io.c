/*
   Copyright (C) 2019 Shawwwn <shawwwn1@gmail.com>
   GNU General Public License
*/


#include <unistd.h>
#include <stdbool.h> 
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"

static inline bool is_hex(const char *str)
{
	return (str[0] == '0' && str[1] == 'x' && str[2] != '\0');
}

int ioctl_main(int argc, char **argv)
{
	// check arguments format
	if (strcmp(argv[1], "-h") == 0) {
		builtin_usage();
		return EX_USAGE;
	} else if (strcmp(argv[1], "--help") == 0) {
		builtin_help();
		return EX_USAGE;
	} else if (argc<3 || argc>4) {
		builtin_usage();
		return 2;
	}

	// parse arguments
	int fd = strtol(argv[1], NULL, 10);
	if (errno != 0) {
		perror("Invalid fd!\n");
		return 1;
	}

	unsigned int request = (int) (
		(is_hex(argv[2])) ?
		strtol(argv[2], NULL, 16) :
		strtol(argv[2], NULL, 10));
	if (errno != 0) {
		perror("Invalid request code!\n");
		return 1;
	}

	void *message = NULL;
	if (argc == 4) {
		message = argv[3];
	}

	// make syscall
	int ret = ioctl(fd, request, message);
	if (ret != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
	}
	return ret;
}

int ioctl_builtin(WORD_LIST *list)
{
	char **v;
	int c, r;

	v = make_builtin_argv(list, &c);
	r = ioctl_main(c, v);
	free(v);

	return r;
}

char *ioctl_doc[] = {
	"",
	"syscall: ioctl()",
	"http://man7.org/linux/man-pages/man2/ioctl.2.html",
	"",
	"Does NOT support passgeing struct pointer to the third argument ATM.",
	(char *)NULL
};

struct builtin ioctl_struct = {
	.name           = "ioctl",
	.function       = ioctl_builtin,
	.flags          = BUILTIN_ENABLED,
	.long_doc       = ioctl_doc,
	.short_doc      = "ioctl FD REQUEST [MESSAGE]",
	.handle         = NULL,
};
