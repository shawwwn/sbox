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

# Run htop in 'sandbox1', monitor processes in that sandbox
sbox-mgt join -n sandbox1 htop
```

### ACTION:

* **stop**

    Kill sandbox'ed program, stop a running container.\
    Specify whether to commit/merge changes with flags `--auto-commit|--auto-merge`.
    
* **join**

    *sbox*'s default policy forbids creating a container when there is one running under the same name.\
    This flag allows *sbox* to send progam (specified in `ARGS`) into in an existing container environment.\
    Any program sent by this flag will be forcibly terminated(SIGKILL) when the main program exits.

* **list**

    List running/active containers.\
    When use with `--container-name`, list only target container.
    When use with `--verbose`, print more information about target container.\
    When use with `--list-all`, list all containers.

* **remove**
    
    Remove all related files of target container.


### OPTION:

* **-n | --container-name**

    Name of the target container.
    Default: default

* **-c | --auto-commit**

    Auto-commit file system changes to snapshot directory when exits from container.
    
* **-m | --auto-merge**

    Auto-merge contents in snapshot directory to the rootfs outside container when exits from container.

* **-a | --list-all**

    List all containers, including ones that are not running.

* **-v | --verbose**

    Print more information.
    
* **-h | --help**

    Display help information.


### Note:

If not used with flag `--auto-commit|--auto-merge`, default action will be to discard filesystem changes.
