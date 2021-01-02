#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>
#include<linux/slab.h>
#include "pidtoname.h"


asmlinkage long sys_pidtoname(int pid, char* buf, int len){

    struct task_struct *task;
    char *process_name = kmalloc(60, GFP_KERNEL);

    for_each_process(task){  
    	//compare to each task is running
	if (task_pid_nr(task) == pid){
		//compare process name
  		strcpy(process_name, task->comm); 
		if(strlen(task->comm) <= 60){
  			process_name[strlen(task->comm)] = 0;
		}

		printk("Process Name :=%s\n", process_name);
		copy_to_user(buf, process_name, len - 1);
		
		if(strlen(process_name) > len - 1){
			return strlen(process_name); 
		}
		else return 0;
	}
    }
    return -1; 
}
