#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/sched.h> //<sched.h> library method that iterates through list of processes from task_struct 
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/string.h>
#include "pnametoid.h"

asmlinkage long sys_pnametoid(char* name){

	//tasklist struct to use
	struct task_struct *task;
	//tty struct
	struct tty_struct *my_tty;
	//get current tty
	my_tty = get_current_tty();
	//placeholder to print full string to tty
	char cname[32];

	for_each_process(task){
		//compares the current process name (defined in task->comm) to the passed in cname
		if(strcmp(task->comm,cname) == 0){
			//convert to string and put into cname[]
			sprintf(cname, "PID = %ld\n", (long)task_pid_nr(task));
			//show result to user that called the syscall
			(my_tty->driver->ops->write) (my_tty, cname, strlen(cname)+1);
			// return pid
			return task_pid_nr(task); 
		}
	}
	return -1;
}
