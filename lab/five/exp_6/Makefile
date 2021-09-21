
BASEINCLUDE ?= ../../../

block_demo-objs := demo_block.o
obj-m := block_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;