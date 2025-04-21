pc: pc.c eventbuf.c 
	gcc -Wall -Wextra -o $@ $^ -lpthread

pc.zip:
	rm -f $@
	zip $@ Makefile pc.c eventbuf.c 