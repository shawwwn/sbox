A shared library to be dynamically loaded by bash with 'enable -f' and create new builtin command(s).
Builtin command allows calling syscalls without first invoking fork() or execv() therefore preserves current process's execution context.

## Usages:
```bash
enable -f sbox-seccomp.so seccomp

# 160 is the reference number of `uname` in aarch64 platform.
# Blacklist syscall `uname`.
# No longer able to gain new privileges after `--no-new-privs`.
seccomp -a blacklist --no-new-privs 160 
uname # will print permission denied
```

## seccomp:

```
seccomp [-a blacklist|whitelist] [-v] [--no-new-privs] syscall_no1 syscall_no2 ...
```

