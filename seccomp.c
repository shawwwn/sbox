/*
   Copyright (C) 2019 Shawwwn <shawwwn1@gmail.com>
   GNU General Public License
*/


#include <unistd.h>
#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/prctl.h>
#include <linux/seccomp.h>
#include <seccomp.h>
#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"

#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"

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
	bool no_new_privs = false;

	struct optparse_long longopts[] = {
		{"no-new-privs", 'N', OPTPARSE_NONE},
		{"action",       'a', OPTPARSE_REQUIRED},
		{"version",      'v', OPTPARSE_NONE},
		{"help",         'h', OPTPARSE_NONE},
		{0}
	};
	int option;
	struct optparse options;
	optparse_init(&options, argv);
	while ((option = optparse_long(&options, longopts, NULL)) != -1) {
		switch (option) {
			case 'N':
				no_new_privs = true;
				break;
			case 'a':
				if (strcmp(options.optarg, "whitelist") == 0) {
					seccomp_action = SCMP_ACT_ALLOW;
				} else if (strcmp(options.optarg, "blacklist") == 0) {
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
				builtin_usage();
				return EX_USAGE;
			case 'h':
			default:
				builtin_help();
				return EX_USAGE;
		}
	}

	// deny gaining new privileges from now on
	// https://www.kernel.org/doc/Documentation/prctl/no_new_privs.txt
	if (no_new_privs) {
		if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
			fprintf(stderr, "Set no_new_privs failed!\n");
			return 1;
		}
		return 0;
	}

	// check if has positional arguments
	if (options.optind >= argc) {
		fprintf(stderr, "No positional args!\n");
		builtin_usage();
		return EX_USAGE;
	}

	// check argument format
	int optind = options.optind; // backup cursor
	char *arg;
	while ((arg = optparse_arg(&options))) {
		if (!isNumber(arg)) {
			fprintf(stderr, "Invalid Argument \"%s\"\n", arg);
			return EINVAL;
		}
	}
	options.optind = optind; // restore cursor

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

	while ((arg = optparse_arg(&options))) {
		int syscall_no = atoi(arg);
		rc = seccomp_rule_add(ctx, seccomp_action, syscall_no, 0);
		if (rc < 0) 
			fprintf(stderr, "Failed to add syscall %u to seccomp.\n", syscall_no);
	}

	rc = seccomp_load(ctx);
	if (rc < 0) {
		fprintf(stderr, "Failed to load seccomp.\n");
		goto out;
	}

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
	"",
	"Block syscall using seccomp.",
	"",
	"e.g.,",
	"seccomp -a blacklist 160 # deny calls to uname() for aarch64 platform"
	"",
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
	.short_doc      = "seccomp [-a blacklist|whitelist] [-v] [--no-new-privs] syscall_no1 syscall_no2 ...",
	.handle         = NULL,
};
