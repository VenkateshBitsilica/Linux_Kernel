#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>


#define CHAR_FILE	"/dev/waitq_dev"

int fd;

void* reader_thread(void *arg)
{
	char buffer[1024];
	printf("Reading data from %s\n",CHAR_FILE);
	ssize_t n = read(fd, buffer, sizeof(buffer));
	if(n>0)
	{
		buffer[n] = '\0';
		printf("Buffer data : %s\n",buffer);
	}
	else
		printf("Failed to read from buffer\n");
	return NULL;
}

void* writer_thread()
{
	sleep(3);
	const char *msg = "Hello from Writer thread!....";
	ssize_t n = write(fd, msg, strlen(msg));
    	if (n > 0) {
        	printf("[Writer] Wrote to device: %s\n", msg);
    	}
       	else
	        printf("Failed to write data to %s\n",CHAR_FILE);

	return NULL;
}
int main()
{
	pthread_t t1, t2;
	
	fd = open(CHAR_FILE,O_RDWR);
	if(fd < 0)
	{
		perror("Failed to oprn Device\n");
		return EXIT_FAILURE;
	}	

	pthread_create(&t1, NULL, reader_thread, NULL);
	pthread_create(&t2, NULL, writer_thread, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	close(fd);

	return 0;
}
