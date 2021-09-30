
BASEINCLUDE ?= ../../../

alloc_slab-objs := slab_demo.o
obj-m := alloc_slab.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;