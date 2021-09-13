# 部分无法拼接出uname -r，可以uname -r的结果替代，
# 如果没有找到这个目录，那就是内核版本和内核header版本不一致，基本可以跳过这个实验，因为即使编译通过也无法使用
BASEINCLUDE ?= /lib/modules/5.4.0-80-generic/build

#注意点： hello.o必须和写的文件hello.c一致，这里应该是利用了make的自动推导功能
hello_module-objs := hello.o
obj-m := hello_module.o

all:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) modules;

clean:
	$(MAKE) -C $(BASEINCLUDE) M=$(PWD) clean;
	rm -rf *.ko;