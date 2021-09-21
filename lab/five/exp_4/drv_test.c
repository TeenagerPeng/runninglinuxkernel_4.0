#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

//#define DEV_NAME "/dev/demo_drv"
#define DEV_NAME "/dev/demo_kfifo"
int main() {
    char buf[64];
    int fd;
    int ret;
    size_t len;
    char message[] = "Testing the virtual FIFO device";
    char *read_buffer;
    len = sizeof(message);

    printf("len = %d", len);

    fd = open(DEV_NAME, O_RDWR);
    if (fd < 0) {
        printf("open dev %s failed!!!\n", DEV_NAME);
        return -1;
    } else {
        printf("open device %d succeed fd \n", fd);
    }


    read_buffer = malloc(2 * len);
    memset(read_buffer, 0, 2*len);
    ret = read(fd, read_buffer, 2*len);
    printf("read %d bytes\n", ret);

    //1. write message to device
    ret = write(fd, message, len);
    if (ret != len) {
        printf("write device %d failed ret = %d\n", fd, ret);
        return -1;
    }
    //close(fd);

    read_buffer = malloc(2 * len);
    memset(read_buffer, 0, 2*len);
    //fd = open(DEV_NAME, O_RDWR);
    
    // if (fd < 0) {
    //     printf("open dev %s failed!!!\n", DEV_NAME);
    //     return -1;
    // } else {
    //     printf("open device %d succeed fd \n", fd);
    // }
    
    ret = read(fd, read_buffer, 2*len);
    printf("read %d bytes\n", ret);
    printf("read buffer = %s \n", read_buffer);
    close(fd);
    return 0;
}
