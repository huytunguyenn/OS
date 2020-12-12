#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>  
#include <fcntl.h>   


int main(){
	int rng;

	// file open for read-only
	int fd = open("/dev/RANDOM_NUMBER", O_RDONLY); // file description 
	if (fd < 0){
		perror("Failed to read file");
		return errno;
	}

	// read file
	read(fd, &rng, sizeof(rng));
	
	printf("Random number: %d\n", rng);
	return 0;       	
}
