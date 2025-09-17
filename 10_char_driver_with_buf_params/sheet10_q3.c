#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    char buffer[100];
    ssize_t r;
    int fd = open("/dev/chardev1", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

   	write(fd, "message1 from user space", 24);
	write(fd, "message2 from user space", 24);
	write(fd, "message3 from user space", 24);

	while((r = read(fd, buffer, sizeof(buffer)-1)) > 0)
	{
		buffer[r] = '\0';
		printf("%s\n",buffer);
		memset(buffer,0,sizeof(buffer));
	}

    close(fd);
    return 0;
}

