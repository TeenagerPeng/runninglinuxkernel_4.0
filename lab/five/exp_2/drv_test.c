#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

//#define DEV_NAME "/dev/demo_drv"
#define DEV_NAME "/dev/demo_misc"
int main() {
    char buf[64];
    int fd;

    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open dev %s failed!!!", DEV_NAME);
        return -1;
    }
    
    int ret = read(fd, buf, 64);
    close(fd);
    return 0;
}
