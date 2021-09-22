
BASEINCLUDE ?= ../../../

poll_demo-objs := demo_poll.o
obj-m := poll_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;