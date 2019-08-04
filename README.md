# sBox

Run programs in a sandbox environment without writting to rootfs.

```bash
# sbox bash
create new snapshot at /tmp/sbox/default/snapshot
Enter container "default":
# pwd
/root
# ls
# mkdir xxxx          # create directory in sandbox
# cd xxxx/
# touch TEST          # create file in sandbox
#
# exit
exit
dirty
└── root
    ├── .bash_history
    └── xxxx
        └── TEST

2 directories, 2 files
36K data generated.
> Commit changes to snapshot [y/n]? Y
commited to /tmp/sbox/default/snapshot
Snapshot size 36K (2 files, 2 directories).
> Merge snapshot with local filesystem [y/n]? N
#
# pwd
/root
# ls                  # nothing gets written to our rootfs
```


## Description:

Programs run in **sBox** still have access to your rootfs except all modifications to file system will be cached. You may chose to merge these changes on exit.

In addition, each sandbox have its own network stack(has internet access), mount points, process info, ipc, uts, and cgroup.

It's more like a stripped down version of docker.

## `sbox` Usage: 

Run program in sandbox

```bash
sbox [OPTION] PROGRAM
```

e.g.,

```bash
# run program in a privileged sandbox
sbox apt install curl

# run program in a un-privileged sandbox, with privileges of user 'shawwwn'
sbox -r shawwwn bash
```
#### OPTION:
* **-n | --container-name NAME**
    
    Name of the sandbox.\
    For identify different sandbox environments. \
    A environment is basically a overlay filesystem on top your existing rootfs.\
    Several working directories will be created under `/tmp/sbox/NAME/`. \
    Default: 'default'

* **-r | --user-root USER** 
    
    Still need root to do initialization. \
    Once initialization is done, enter *user namespace* as user `USER`.\
    Finally, map `USER`'s uid to root inside sandbox.\
    Program(s) run inside our sandbox will appear to be running as root but in fact only has `USER`'s privilege.\
    Default: 'root'

* **-d | --snapshot-dir DIR** 

    Path of the snapshot directory storing **commited** file system changes after a program exits from sandbox.\
    Default: 'snapshot'    # relative path inside /tmp/sbox/NAME/
    
* **-v | --verbose**

    Print more information.
    
* **-h | --help**

    Display help information.


## `sbox-fstool` Usage: 

Tool for commit, prune, merge files created in sandbox environment by sbox.

```bash
sbox ACTION NAME [OPTION]
```

e.g.,
```bash
# commit to default environment
sbox-fstool commit

# suppose we want to prune sandbox call 'sb2' that has its snapshot directory at '/mnt/usb0/backup'
ssbox-fstool prune sb2 --snapshot-dir /mnt/usb0/backup --verbose

# merge default sandbox to rootfs located at '/mnt/usb0/rootfs'
ssbox-fstool merge default --rootfs-dir /mnt/usb0/rootfs
```

#### ACTION:

* **commit**
    
    Move uncommitted files from */tmp/sbox/NAME/dirty* to `SNAPS_DIR` directory.
    
* **prune**

    Remove sandbox `NAME`'s identical files in `SNAPS_DIR` if they are also in `ROOTFS_DIR`.\
    Help to reduce snapshot size.

* **merge**

    Merge changes from `SNAPS_DIR` to `ROOTFS_DIR` for sandbox `NAME`.\
    If you want to make changes permanent.

#### NAME:

Please refer to `--container-name NAME` above.\
Default: default

#### OPTION:

* **-s | --snapshot-dir SNAPS_DIR**

    Used by action {*commit*, *prune*, *merge*}.\
    Default: /tmp/sbox/<NAME>/sandbox
    
* **-r | --rootfs-dir ROOTFS_DIR**

    Used by action {*prune*, *merge*}.\
    Default: /

* **-v | --verbose**

    Print more information.
    
* **-h | --help**

    Display help information.


## Note:

- sBox use `OverlayFS` as the "storage engine" (docker jargon ><). Files generated by your program will be cached at `/tmp/sbox/CONTAINER NAME/dirty`. Upon ***commit***, these files will be move to `SNAPSHOT DIR` overriding any existing one.

- Can not map current user to arbitary user in sandbox. Mapping is no longer do-able using pure shell script because newer kernel doesn't allow updating uid_map/gid_map after 'execve'[(\*)](https://stackoverflow.com/questions/45972426/unshare-user-namespace-and-set-uid-mapping-with-newuidmap).

- If `-r USER` is not set, sBox will not create custom *user namespace*, so all other namespaces will be children of the namespace the invoking process is in(e.g., shell's user namespace). Do not set this flag if custom *user namespace* breaks your program. On the other hand, if this flag is set, all other namespaces will be children to the newly created *user namespace*.
