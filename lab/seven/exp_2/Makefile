
BASEINCLUDE ?= ../../../

get_physical_page_demo-objs := get_physical_page.o
obj-m := get_physical_page_demo.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;