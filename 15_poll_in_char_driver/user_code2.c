// poll_user.c
#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/waitq_dev", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct pollfd pfd = { .fd = fd, .events = POLLOUT };

    printf("waiting for poll to write data\n");
    int ret = poll(&pfd, 1, 10000); 

    if (ret == 0) {
        printf("Timeout!\n");
    } else if (pfd.revents & POLLOUT) {
        char buf[256] = "POLLOUT implemented for accepting write";
        write(fd, buf, sizeof(buf));
    }

    close(fd);
    return 0;
}
