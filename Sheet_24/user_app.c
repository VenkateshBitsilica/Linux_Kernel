#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>

#define GPIOBTN_RESET		_IO('V',0)

int main()
{
	int fd = open("/dev/gpio_btn", O_RDWR);

	if(fd<1)return 1;

	char buf[64];

	read(fd,buf,sizeof(buf));

	printf("%s\n",buf);

	write(fd, "1", 1); 
    	ioctl(fd, GPIOBTN_RESET);

    	close(fd);
	return 0;
}
