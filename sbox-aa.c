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

#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"
#include <sys/apparmor.h>


//*****************************************************************************
//*
//* aa_change_profile PROFILE_NAME1 PRFILE_NAME2 ...
//*
//*****************************************************************************

int aa_change_profile_builtin(WORD_LIST *list) {
	reset_internal_getopt ();

	int opt;
	while ((opt = internal_getopt(list, "h")) != -1) {
		switch (opt) {
			CASE_HELPOPT;
			default:
				builtin_usage();
				return (EX_USAGE);
		}
	}
	list = loptend;

	if (list == 0) {
		builtin_usage();
		return (EX_USAGE);
	}

	WORD_LIST *l;
	for (l=list; l; l=l->next) {
		if ((opt=aa_change_profile(l->word->word)) != EXECUTION_SUCCESS) {
			builtin_error("Change to profile \"%s\" failed. (%d)", l->word->word, opt);
			continue;
		}
	}

	return opt;
}

char *aa_change_profile_doc[] = {
	"",
	"To change into a new apparmor profile, immediately.",
	"",
	"Profiles must be loaded into kernel prior to calling this function.",
	"http://manpages.ubuntu.com/manpages/bionic/man2/aa_change_profile.2.html",
	(char *)NULL
};

/*
 * Change current process's apparmor profile immediately.
 */
struct builtin aa_change_profile_struct = {
	.name           = "aa_change_profile",
	.function       = aa_change_profile_builtin,
	.flags          = BUILTIN_ENABLED,
	.long_doc       = aa_change_profile_doc,
	.short_doc      = "aa_change_profile PROFILE_NAME",
	.handle         = NULL,
};




//*****************************************************************************
//*
//* aa_remove_profile PROFILE_NAME1 PRFILE_NAME2 ...
//*
//*****************************************************************************

int aa_remove_profile_builtin(WORD_LIST *list) {
	reset_internal_getopt ();

	int opt;
	while ((opt = internal_getopt(list, "h")) != -1) {
		switch (opt) {
			CASE_HELPOPT;
			default:
				builtin_usage();
				return (EX_USAGE);
		}
	}
	list = loptend;

	if (list == 0) {
		builtin_usage();
		return (EX_USAGE);
	}

	aa_kernel_interface *kern_if;
	opt = aa_kernel_interface_new(&kern_if, NULL, NULL);
	if (opt != EXECUTION_SUCCESS) {
		builtin_error("Error accessing AppArmor module.");
		return opt;
	}

	WORD_LIST *l;
	for (l=list; l; l=l->next) {
		if ((opt=aa_kernel_interface_remove_policy(kern_if, l->word->word)) != EXECUTION_SUCCESS) {
			builtin_error("Remove profile \"%s\" failed. (%d)", l->word->word, opt);
			continue;
		}
	}

	aa_kernel_interface_unref(kern_if);

	return opt;
}

char *aa_remove_profile_doc[] = {
	"",
	"Remove a apparmor profile from kernel.",
	"",
	"Profiles must be loaded into kernel prior to calling this function.",
	(char *)NULL
};

/*
 * Remove a apparmor profile from kernel.
 */
struct builtin aa_remove_profile_struct = {
	.name           = "aa_remove_profile",
	.function       = aa_remove_profile_builtin,
	.flags          = BUILTIN_ENABLED,
	.long_doc       = aa_remove_profile_doc,
	.short_doc      = "aa_remove_profile PROFILE_NAME",
	.handle         = NULL,
};


