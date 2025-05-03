.PHONY: libvvsfs.a clean pristine testfs test

#static library
libvvsfs.a: testfs.o image.o block.o
	ar rcs $@ $^

#object files
testfs.o: testfs.c image.h block.h ctest.h
	gcc -Wall -Wextra -Werror -c -o $@ testfs.c

image.o: image.c image.h block.h
	gcc -Wall -Wextra -Werror -c -o $@ image.c

block.o: block.c block.h image.h
	gcc -Wall -Wextra -Werror -c -o $@ block.c

#link final executable
testfs: testfs.o libvvsfs.a
	gcc -o $@ $^

#run tests
test: testfs
	./testfs

#cleans objects
clean:
	rm -f *.o
#clean everything 
pristine:
	rm -f *.o testfs .a

