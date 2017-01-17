

 
CC=arm-none-linux-gnueabi-gcc
AR=arm-none-linux-gnueabi-ar
STRIP=arm-none-linux-gnueabi-strip


all:
	$(CC) -c gui.c
	$(AR) rcu libgui.a epoll.o general.o utils.o zmalloc.o gui.o



clean:
	@rm -f gui.o
	@rm -f libgui.a


