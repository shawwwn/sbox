/* Bash4 builtin to be dynamically loaded with enable -f and create a new
   builtin. */

/*
   Copyright (C) 2019 Shawwwn <shawwwn1@gmail.com>
   GNU General Public License
*/

#include <unistd.h>
#include <stdint.h>
#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <seccomp.h>
#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"

extern char **make_builtin_argv ();
extern void builtin_usage __P((void));

bool isNumber(char *str)
{
	for (int i=0; str[i]!='\0'; i++) 
	{ 
		if (!isdigit(str[i])) {
			return false;
		}
	}
	return true;
}

int seccomp_main (int argc, char **argv)
{
	unsigned int seccomp_action = SCMP_ACT_ERRNO(EPERM); // blacklist by default

	// parse arguments
	int c;
	while ((c = getopt (argc, argv, "a:v")) != -1) {
		switch (c) {
			case 'a':
				if (strcmp(optarg, "whitelist") == 0) {
					seccomp_action = SCMP_ACT_ALLOW;
				} else if (strcmp(optarg, "blacklist") == 0) {
					seccomp_action = SCMP_ACT_ERRNO(EPERM);
				} else {
					builtin_usage();
					return EX_USAGE;
				}
				break;
			case 'v':
				printf("1.0 - 2019 Shawwwn <shawwwn1@gmail.com>\n");
				return 0;
				break;
			case '?':
			default:
				builtin_usage();
				return EX_USAGE;
				break;
		}
	}

	// check argument format
	for (int i=optind; i<argc; i++) {
		char* arg = argv[i];
		if (!isNumber(arg)) {
			fprintf(stderr, "Invalid Argument \"%s\".\nAbort.\n", arg);
			return EINVAL;
		}
	}

	if (optind >= argc) {
		builtin_usage();
		return EX_USAGE;
	}

	// filter syscall
	int rc = -1;
	scmp_filter_ctx ctx;
	ctx = seccomp_init(
		seccomp_action==SCMP_ACT_ALLOW ?
		SCMP_ACT_ERRNO(EPERM) : // deny by default, whitelist
		SCMP_ACT_ALLOW // allow by default, blacklist
	);
	if (ctx == NULL)
		goto out;

	for (int i=optind; i<argc; i++) {
		int syscall_no = atoi(argv[i]);
		rc = seccomp_rule_add(ctx, seccomp_action, syscall_no, 0);
		if (rc < 0) 
			fprintf(stderr, "Failed to add syscall %u to seccomp.\n", syscall_no);
	}

	rc = seccomp_load(ctx);
	if (rc < 0) {
		fprintf(stderr, "Failed to load seccomp.\n");
		goto out;
	}

	printf("done\n");
	return 0;

out:
	seccomp_release(ctx);
	return -rc;
}

int seccomp_builtin(WORD_LIST *list)
{
	char **v;
	int c, r;

	v = make_builtin_argv(list, &c);
	r = seccomp_main(c, v);
	free(v);

	return r;
}

char *seccomp_doc[] = {
	"Block syscall using seccomp.",
	"\n",
	"e.g.,",
	"seccomp -a blacklist 160 # deny calls to uname() for aarch64 platform"
	"\n",
	"Each syscall is represented by a reference number.",
	"Reference numbers are different under different architectures.",
	"Find the correct reference number in:",
	"https://fedora.juszkiewicz.com.pl/syscalls.html",
	(char *)NULL
};

struct builtin seccomp_struct = {
	.name           = "seccomp",
	.function       = seccomp_builtin,
	.flags          = BUILTIN_ENABLED,
	.long_doc       = seccomp_doc,
	.short_doc      = "seccomp [-a blacklist|whitelist] [-v] syscall_no1 syscall_no2 ...",
	.handle         = NULL,
};
