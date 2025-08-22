#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE "/dev/ioctl_dev"
#define MAJOR_NUM 	'V'
#define IOCTL_RESET_BUFFER 	_IO(MAJOR_NUM, 0)
#define IOCTL_GET_COUNT 	_IOR(MAJOR_NUM, 1, int)
#define IOCTL_SET_DEBUG		_IOW(MAJOR_NUM,2, int)

int main() {
	int debug_mode;
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    char msg[] = "Hello ioctl!";
    write(fd, msg, strlen(msg));

    int count;
    ioctl(fd, IOCTL_GET_COUNT, &count);
    printf("Write count = %d\n", count);
	
    printf("Enter Debug mode value:\n1-Enable\n0-Disable\n");
    scanf("%d",&debug_mode);
    ioctl(fd,IOCTL_SET_DEBUG, &debug_mode);

    ioctl(fd, IOCTL_RESET_BUFFER);
    printf("Buffer reset.\n");

    close(fd);
    return 0;
}


