## *sbox-mgt*

Sandbox instance management.

```bash
sbox-mgt ACTION [OPTION] ARGS
```

### Sample Usages:

```bash
# Stop container 'sbox1', commit filesystem changes to its snapshot directory, merge snapshot to current rootfs.
sbox-mgt stop -n sbox1 -c -m

# Stop container 'default', commit filesystem changes to its snapshot directory.
sbox-mgt stop -c

# Send/Run `htop` into existing container 'sbox2'.
sbox-mgt join -n sbox2 htop
```

### ACTION:

* **stop**

    Kill sandbox'ed program, stop a running sandbox.\
    Specify whether to commit/merge changes with flags -c/-m.
    
* **join**

    *sbox*'s default policy forbids creating a sandbox when there is one running under the same name.\
    This flag allows *sbox* to send progam (specified in `ARGS`) into in an existing sandbox environment.\
    Any program sent by this flag will be forcibly terminated(SIGKILL) when the main program exits.

### OPTION:

* **-n | --container-name**

    Name of the target container.
    Default: default

* **-c | --auto-commit**

    Auto-commit file system changes to snapshot directory when exits from sandbox.
    
* **-m | --auto-merge**

    Auto-merge contents in snapshot directory to the rootfs outside sandbox when exits from sandbox.

* **-v | --verbose**

    Print more information.
    
* **-h | --help**

    Display help information.


### Note:

If not used with flag **-c|-m**, default action will be to discard filesystem changes.

