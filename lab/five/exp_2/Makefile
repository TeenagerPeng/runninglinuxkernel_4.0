
BASEINCLUDE ?= /home/mi/work/linux-kernel/runninglinuxkernel_4.0

miscdev_demo-objs := demo_miscdev.o
obj-m := miscdev_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;