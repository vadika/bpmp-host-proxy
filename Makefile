obj-m += bpmp-host-proxy.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc user-test.c -o user-test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm user-test
