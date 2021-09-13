
BASEINCLUDE ?= /home/mi/work/linux-kernel/runninglinuxkernel_4.0

chardev_demo-objs := demo_chardev.o
obj-m := chardev_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;