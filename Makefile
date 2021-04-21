all:
	gcc -o main -g main.c ntfs.c ntfs_utils.c available_devices.c -lblkid
clean:
	rm main