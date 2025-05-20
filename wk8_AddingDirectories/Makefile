.PHONY: libvvsfs.a clean pristine testfs test

#static library
libvvsfs.a: testfs.o image.o block.o free.o inode.o pack.o dir.o
	ar rcs $@ $^

#object files
testfs.o: testfs.c image.h block.h ctest.h
	gcc -Wall -Wextra -Werror -DCTEST_ENABLE -c -o $@ testfs.c

image.o: image.c image.h block.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ image.c

block.o: block.c block.h image.h free.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ block.c

free.o: free.c free.h image.h block.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ free.c

inode.o: inode.c inode.h block.h free.h pack.h ctest.h 
	gcc -Wall -Wextra -Werror -c -o $@ inode.c

pack.o: pack.c pack.h block.h free.h image.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ pack.c

dir.o: dir.c dir.h block.h inode.h pack.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ dir.c 

#link final executable
testfs: testfs.o libvvsfs.a
	gcc -o $@ $^

#run make test
test: testfs
	./testfs

#cleans objects
clean:
	rm -f *.o
#clean everything 
pristine:
	rm -f *.o testfs libvvsfs.a test.txt 

