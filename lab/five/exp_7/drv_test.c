#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<errno.h>
#include<poll.h>
#include<linux/input.h>
#include<unistd.h>

//#define DEV_NAME "/dev/demo_drv"
#define DEV_NAME "/dev/demo_nonblock"
int main() {
    int ret;
    struct pollfd fds[2];
    char buffer0[64];
    char buffer1[64];

    fds[0].fd = open("/dev/demo_poll0", O_RDWR);
    if (fds[0].fd == -1) {
        goto fail;
    }
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = open("/dev/demo_poll1", O_RDWR);
    if (fds[1].fd == -1) {
        goto fail;
    }
    fds[1].events = POLLIN;
    while(1) {
        ret = poll(fds, 2, -1);
        if (ret == -1) {
            goto fail;
        }
        if (fds[0].revents & POLLIN) {
            ret = read(fds[0].fd, buffer0, 64);
            if (ret < 0) {
                goto fail;
            }
            printf("demo_poll0 read = %s\n", buffer0);
        }
        if (fds[1].revents & POLLIN) {
            ret = read(fds[1].fd, buffer1, 64);
            if (ret < 0) {
                goto fail;
            }
            printf("demo_poll1 read = %s\n", buffer1);
        }
    }
    
    return 0;

fail:
    perror("poll test");
    exit(EXIT_FAILURE);
}
