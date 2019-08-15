# sBox

Run programs in a sandbox environment without writting to rootfs.

```bash
# sbox bash
Create new container "default" with snapshot at /tmp/sbox/default/snapshot
assigned ip 192.168.50.2 for sandbox
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
> Commit changes to snapshot? [Y/n] y
commited to /tmp/sbox/default/snapshot
Snapshot size 36K (1 files, 1 directories).
> Merge snapshot with local filesystem? [N/y] n
Abort.
#
# pwd
/root
# ls                  # nothing gets written to our rootfs
```


## Description:

Programs run in **sBox** still have access to your rootfs except all modifications to file system will be cached. You may chose to merge these changes on exit.

In addition, each sandbox have its own network stack(has internet access), mount points, process info, ipc, uts, and cgroup.

It's more like a stripped down version of docker.

## Install:

```bash
# dependencies, make adjustments for package managers other than apt
apt-get install iproute2 net-tools iptables bash grep diffutils findutils sudo ipcalc pcregrep tree attr libseccomp2 libseccomp-dev
make install # copy executeables to /usr/bin
```

------------------------------------
## *sbox* usage: 

Run program in sandbox

```bash
sbox [OPTION] PROGRAM
```

e.g.,

```bash
# run program in a privileged sandbox
sbox apt install curl

# run program in sandbox as unprivileged user 'shawwwn'
sbox -u shawwwn bash

# run bash in sandbox named 'sandbox1'
sbox -n sandbox1 bash

# run htop in 'sandbox1', monitor processes in sandbox
sbox -n sandbox1 -j htop
```

#### OPTION:

* **-n | --container-name NAME**
    
    Name of the sandbox.\
    For identify different sandbox environments. \
    A environment is basically a overlay filesystem on top your existing rootfs.\
    Several working directories will be created under `/tmp/sbox/NAME/`. \
    Default: 'default'

* **-u | --user-root USER** 
    
    Still need root to do initialization. \
    Once initialization is done, enter *user namespace* as user `USER`.\
    Finally, map `USER`'s uid to root inside sandbox.\
    Program(s) run inside our sandbox will appear to be running as root but in fact only has `USER`'s privilege.\
    Default: 'root'

* **-d | --snapshot-dir DIR** 

    Path of the snapshot directory storing **commited** file system changes after `PROGRAM` exits from sandbox.\
    Default: 'snapshot'    # relative path inside /tmp/sbox/NAME/

* **-B | --seccomp-blacklist SYSCALLS**
    
    Deny syscalls that are in list SYSCALLS inside sandbox via `seccomp`.\
    List SYSCALLS has a format of `'syscall1,syscall2,syscall3'` (no space in between function names).\
    Please refer to *Note* section for default syscall blacklist.

* **-D | --cap-drop CAPS**
    
    Drop capabilities that are in list CAPS inside sandbox.\
    List CAPS has a format of `'cap1,cap2,cap3'` (no space in between cap names).\
    Please refer to *Note* section for default capability drop-list.
    
* **-j | --join**

    *sbox*'s default policy forbids creating a sandbox when there is one running under the same name.\
    This flag allows *sbox* to send `PROGRAM` into in an existing sandbox environment.\
    Any program sent by this flag will be forcibly terminated(SIGKILL) when the main program exits.
    
* **-v | --verbose**

    Print more information.
    
* **-h | --help**

    Display help information.


------------------------------------
## *sbox-fstool* usage: 

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

- Running sandbox inside an existing sandbox will work for the first time; howerever, it will fail if you continue to launch sandbox in a "layer-two" sandbox with errors like 'overlayfs mount failed'. This is not a bug with **sbox** but a limitation(bug?) with OverlayFS[(\*)](https://github.com/rkt/rkt/issues/1537). Currently, OverlayFS can't mount on itself for more than two layers.

- `seccomp` is implemented in C as a bash builtin command(unfortunately there is no way to do this in pure bash). 

    ```bash
    # enable -f seccomp.so seccomp
    # seccomp
    seccomp: usage: seccomp [-a blacklist|whitelist] [-v] syscall_no1 syscall_no2 ...
    ```

    Default security([seccomp](http://man7.org/linux/man-pages/man2/seccomp.2.html)) profile will deny these syscalls once we are in sandbox[(\*)](https://docs.docker.com/engine/security/seccomp/):
    
    `acct, add_key, bpf, clock_adjtime, clock_settime, create_module, delete_module, finit_module, get_kernel_syms, get_mempolicy, init_module, ioperm, iopl, kcmp, kexec_file_load, kexec_load, keyctl, lookup_dcookie, mbind, move_pages, name_to_handle_at, nfsservctl, perf_event_open, pivot_root, process_vm_readv, process_vm_writev, ptrace, query_module, quotactl, reboot, request_key, set_mempolicy, settimeofday, stime, swapon, swapoff, sysfs, _sysctl, uselib, userfaultfd, ustat, vm86, vm86old`
    
- Default security([capabilities](http://man7.org/linux/man-pages/man7/capabilities.7.html)) profile includes dropping these unessential capabilities[(\*)](https://github.com/moby/moby/blob/master/oci/defaults.go#L14-L30):
    
    `cap_dac_read_search, cap_ipc_lock, cap_ipc_owner, cap_lease, cap_linux_immutable, cap_mac_admin, cap_mac_override, cap_net_broadcast, cap_audit_control, cap_audit_read, cap_block_suspend, cap_sys_boot, cap_sys_module, cap_sys_nice, cap_sys_pacct, cap_sys_ptrace, cap_sys_rawio, cap_sys_resource, cap_sys_time, cap_sys_tty_config, cap_syslog, cap_wake_alarm`
