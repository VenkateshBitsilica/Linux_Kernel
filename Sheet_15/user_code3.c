// poll_user.c
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE		"/dev/waitq_dev"
#define READ_READY	_IOR('V',0,int)

int main() {
	int read_status = 1;
    int fd = open("/dev/waitq_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    char buf[256] = "Writing from user space and ioctl used to set READ_READY Flag\n";
    write(fd, buf, sizeof(buf));
    
    ioctl(fd,READ_READY,&read_status);
    struct pollfd pfd = { .fd = fd, .events = POLLIN };

    printf("waiting for poll to read data from driver file\n");
    int ret = poll(&pfd, 1, 10000);

    if (ret == 0) {
        printf("Timeout!\n");
    } else if (pfd.revents & POLLIN) {
        char buf1[256];
        read(fd, buf1, sizeof(buf1));
	printf("Buffer from Driver file : %s\n",buf1);
    }

    close(fd);
    return 0;
}

