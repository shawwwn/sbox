/*
   Copyright (C) 2019 Shawwwn <shawwwn1@gmail.com>
   GNU General Public License
*/

#include <unistd.h>
#include <stdbool.h> 
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"

#define OPTPARSE_IMPLEMENTATION
#include "optparse.h"

extern char **make_builtin_argv ();
extern void builtin_usage __P((void));


int aa_change_profile_main(int argc, char **argv)
{
	return 0;
}

int aa_change_profile_builtin(WORD_LIST *list)
{
	reset_internal_getopt ();
	while ((int opt = internal_getopt (list, "h")) != -1) {
		switch (opt)
		{
			CASE_HELPOPT;
			default:
			builtin_usage ();
			return (EX_USAGE);
		}
	}
	list = loptend;

	if (list == 0) {
		builtin_usage ();
		return (EX_USAGE);
	}

	opt = EXECUTION_SUCCESS;
	WORD_LIST *l;
	for (l = list; l; l = l->next) {
		builtin_error ("Error: %s", l->word->word);
		opt = EXECUTION_FAILURE;
		continue;
	}
	return opt;
}

char *aa_change_profile_doc[] = {
	"To change into a new apparmor profile, immediately.",
	"\n",
	"Profiles must be loaded into kernel prior to calling this function."
	"http://manpages.ubuntu.com/manpages/bionic/man2/aa_change_profile.2.html",
	(char *)NULL
};

struct builtin aa_change_profile_struct = {
	.name           = "aa_change_profile",
	.function       = aa_change_profile_builtin,
	.flags          = BUILTIN_ENABLED,
	.long_doc       = aa_change_profile_doc,
	.short_doc      = "aa_change_profile PROFILE_NAME",
	.handle         = NULL,
};
