all:
	gcc -o main -g main.c ntfs.c ntfs_utils.c available_devices.c -lblkid
clean:
	rm main
lib:
	gcc -fPIC -Wall -g -c main.c ntfs.c ntfs_utils.c available_devices.c -lblkid
	gcc -g -shared -Wl,-soname,libntfsdr.so.0 -o libntfsdr.so.0.0 main.o ntfs.o ntfs_utils.o available_devices.o -lc

clean_lib:
	rm *.o *.so.0.0