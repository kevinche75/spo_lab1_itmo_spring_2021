# NTFS DRIVER

## List mode

Show available block devices

Example:

```
System partition:  
/dev/nvme0n1p1	300 MiB		TYPE=vfat	UUID=3B9D-FCC3  
/dev/nvme0n1p2	238 GiB		TYPE=ext4	UUID=31e51bfd-ada4-4ef1-b37e-b9f75a257a6b  
/dev/loop0	64 MiB		TYPE=squashfs  
/dev/loop1	32 MiB		TYPE=squashfs  
/dev/loop2	32 MiB		TYPE=squashfs  
/dev/loop3	64 MiB		TYPE=squashfs  
/dev/loop4	99 MiB		TYPE=squashfs  
/dev/loop5	55 MiB		TYPE=squashfs  
/dev/loop6	55 MiB		TYPE=squashfs  
/dev/loop7	99 MiB		TYPE=squashfs  
/dev/loop8	1000 MiB		TYPE=ntfs	UUID=0BD751A54164DC5D
```

## Shell mode

1. cd [directory] - change working directory
2. pwd - print working directory full name
3. cp - [directory] [target directory] - copy dir or file from mounted device
4. ls - show working directory elements
5. exit - terminate program
6. help - print help