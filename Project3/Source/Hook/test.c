#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

int main(){

	int fd = open("in.txt",O_WRONLY | O_CREAT | O_APPEND);
	
	if(write(fd, "18120254\n", 9) == 9) {
    		printf("Write successfully\n");
	}
	else{
    		printf("Write failed\n");
	}
	return 0;
}


