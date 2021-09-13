
BASEINCLUDE ?= /home/mi/work/linux-kernel/runninglinuxkernel_4.0

vchardev_demo-objs := demo_vchardev.o
obj-m := vchardev_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;