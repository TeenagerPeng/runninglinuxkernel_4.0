# init.h 和 module.h目录
BASEINCLUDE ?= /home/mi/work/linux-kernel/runninglinuxkernel_4.0

#注意点： hello.o必须和写的文件hello.c一致，这里应该是利用了make的自动推导功能
hello_module-objs := hello.o
obj-m := hello_module.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;