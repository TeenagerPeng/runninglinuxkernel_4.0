
BASEINCLUDE ?= ../../../

alloc_page-objs := alloc_page_demo.o
obj-m := alloc_page.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;