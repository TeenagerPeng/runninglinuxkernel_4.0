
BASEINCLUDE ?= ../../../

kfifo_demo-objs := demo_kfifo.o
obj-m := kfifo_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;