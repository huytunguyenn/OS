KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd`
	$(CC) userTest.c -o userTest
clean:
	make -C $(KDIR) M=`pwd` clean
	rm userTest
