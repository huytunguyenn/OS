#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>

#define MAX 32

int main(){
	char name[MAX]; 
	int pid;
	puts("Enter process id to find: ");
	scanf("%d",pid);
    	int status = syscall(321, pid, name, MAX);//syscall number 321 and passing in the pid int
    	
    	if(status != -1){
    		printf("Status: Success!\n");
    		printf("PID: %d\n, Process' name: %s\n",pid,name);
    	}
    	else{
    		printf("Status: Process' name not found!\n");
    	}
    
	return 0;
}
