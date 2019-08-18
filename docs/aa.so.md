A shared library to be dynamically loaded by bash with 'enable -f' and create new builtin command(s).
Builtin command allows calling syscalls without first invoking fork() or execv() therefore preserves current process's execution context.

## Usages:
```bash
apparmor_parser some.profile # profile file contains independent profile name 'some_profile'
enable -f sbox-aa.so aa_remove_profile aa_remove_profile

# dynamically change current process's apparmor profile to 'some_profile'
aa_change_profile some_profile 

# remove 'some_profile' from kernel
aa_remove_profile some_profile
```

## aa_change_profile:

```
aa_change_profile PROFILE_NAME
```

Calling aa_change_profile() without fork.
http://manpages.ubuntu.com/manpages/bionic/man2/aa_change_profile.2.html

## aa_remove_profile

```
aa_remove_profile PROFILE_NAME
```

Calling aa_kernel_interface_remove_policy() without fork.
