# sBox

Run programs in a sandbox environment without writting to rootfs.

<img src="https://user-images.githubusercontent.com/4016736/63220940-4de16780-c146-11e9-84ee-e643a0f9e83d.png" width="250" height="250" />

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

In addition, **sBox** is capable of doing the following:

- [x] Namespaces
    - [x] Internet Access
    - [x] Mount Points
    - [x] PID (process view)
    - [x] IPC (shared memory between processes)
    - [x] UTS (hostname)
    - [x] CGroup
- [x] CGroup Integration (cgroup controllers on sandbox main process)
- [x] Seccomp(syscall) Blacklist
- [x] Capability Blacklist
- [x] AppArmor Integration (profile per sandbox)

The [default sercurity profile](docs/sbox.md#note) of **sBox** can be a bit aggressive, you may want to tone it down if your program fails to run.

## Installation:

```bash
# dependencies, make adjustments for package managers other than apt
apt-get install iproute2 net-tools iptables bash grep diffutils findutils sudo ipcalc pcregrep tree attr libseccomp2 libseccomp-dev cgroup-tools
make install # copy executeables to /usr/bin
```


## Usage:

* [sbox](docs/sbox.md)
* [sbox-fstool](docs/sbox-fstool.md)
* [sbox-seccomp.so](docs/sbox-seccomp.so.md)
* [sbox-aa.so](docs/sbox-aa.so.md)
