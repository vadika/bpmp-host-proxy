obj-m += bpmp-host-proxy.o
KERNEL_INCLUDES = \
                   -I/usr/src/linux-headers-5.10.104-tegra-ubuntu20.04_aarch64/kernel-5.10/include/ \
				   -I/usr/src/linux-headers-5.10.104-tegra-ubuntu20.04_aarch64/kernel-5.10/arch/arm64/include/ \
				   -I/usr/src/linux-headers-5.10.104-tegra-ubuntu20.04_aarch64/kernel-5.10/arch/arm64/include/generated/ \
				   -I/usr/include/ 
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc user-test.c  $(KERNEL_INCLUDES) -o user-test 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm user-test
