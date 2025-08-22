#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    char buffer[100];
    int fd = open("/dev/simple_char_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    write(fd, "Testing from user space", 24);

    int n = read(fd, buffer, sizeof(buffer));
    buffer[n] = '\0'; 

    printf("Read from driver: %s\n", buffer);

    close(fd);
    return 0;
}


