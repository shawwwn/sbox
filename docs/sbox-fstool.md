## *sbox-fstool*

A tool to commit, prune, merge filesystem image created by sbox.

```bash
sbox-fstool ACTION NAME [OPTION]
```

### Sample Usages:

```bash
# commit to default environment
sbox-fstool commit

# suppose we want to prune sandbox call 'sb2' that has its snapshot directory at '/mnt/usb0/backup'
ssbox-fstool prune sb2 --snapshot-dir /mnt/usb0/backup --verbose

# merge default sandbox's snapshot to rootfs ==> "/", applying all changes to disk
ssbox-fstool merge default
```

### ACTION:

* **commit**
    
    Move uncommitted files from */tmp/sbox/NAME/dirty* to `SNAPS_DIR` directory.
    
* **prune**

    Remove sandbox `NAME`'s identical files in `SNAPS_DIR` if they are also in `ROOTFS_DIR`.\
    Help to reduce snapshot size.

* **merge**

    Merge changes from `SNAPS_DIR` to `ROOTFS_DIR` for sandbox `NAME`.\
    If you want to make changes permanent.

### NAME:

Please refer to `--container-name NAME` above.\
Default: default

### OPTION:

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


### Note:

- All disk I/O inside sandbox will be cached to `dirty` directory of your sandbox work directory in /tmp. Files in `dirty` will be copied to permenant storage `snapshot` when you **commit**. If you decide to apply all changes made in sandbox to disk, you can **merge** that sandbox's `snapshot` with a rootfs of your choice. 

- OverlayFS employs copy-on-write strategy, meaning once you edit a file, a copy of the file containing your edits will be created in the upper layer of the overlay mount. Even if you retore that file to its original state, the copy created earlier will stay in the upper layer despite it is virtually identical to its counterpart. **prune** function will find and delete these identical files.

