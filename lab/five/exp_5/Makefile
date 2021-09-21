
BASEINCLUDE ?= ../../../

nonblock_demo-objs := demo_nonblock.o
obj-m := nonblock_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;