#include <unistd.h>     
#include <stdio.h>      
#include <string.h>     
#include <stdlib.h>     
#include <fcntl.h>   
#include <sys/wait.h>     


#define MAX_LENGTH 100
#define BUF_SIZE 50
#define MAX_HISTORY 30
#define MAX_COMMAND_NAME 30


void readline(char line[MAX_LENGTH])
{
    // Read input
    while (fgets(line, MAX_LENGTH, stdin) == NULL) 
    {
        printf("Cannot read user input!\n");
        fflush(stdin);
    }

    // Delete \n
    line[strcspn(line, "\n")] = '\0';
}

bool history_feature(char *history[], int &count_history, char* line) 
{
    bool check_continue = false;
    // Check input !! (history)
    if (strcmp(line, "!!") == 0) 
    {
        if (count_history == 0) 
        {
            printf("No commands in history\n");
            check_continue = true;
            return check_continue;
        }
        strcpy(line, history[count_history - 1]);
        printf("osh>%s\n", line);
    }

    // If overflow history memory, overwrite memory history[0]
    if (count_history < MAX_HISTORY) 
    {
        strcpy(history[count_history++], line);
    } 
    else 
    {
        free(history[0]);
        for (int i = 1; i < MAX_HISTORY; i++) 
        {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[MAX_HISTORY - 1], line);
    }
    return check_continue;
}

void split_line(char line[], char* args[]) 
{
    for (int index = 0; index < BUF_SIZE; index++) 
    {
        args[index] = NULL;
    }

    const char *delim = " ";
    int index = 0;

    char *token = strtok(line, delim);
    while (token != NULL) 
    {
        args[index++] = token;
        token = strtok(NULL, delim);
    }
    args[index] = NULL;
}

void split_redirection(char* args[], char* redirec_args[]) 
{
    int index = 0;
    redirec_args[0] = NULL;
    redirec_args[1] = NULL;

    while (args[index] != NULL) 
    {
        // Check input: < or >
        if (strcmp(args[index], "<") == 0 || strcmp(args[index], ">") == 0) 
        {
            if (args[index + 1] != NULL) 
            {
                // Move redirect type and file name to redirect arguments vector
                redirec_args[0] = strdup(args[index]);
                redirec_args[1] = strdup(args[index + 1]);
                args[index] = NULL;
                args[index + 1] = NULL;
            }
            
        }
        index++;
    }
}

bool split_pipe(char* args[], char *child1_args[], char *child2_args[]) 
{
    unsigned index = 0, split_index = 0;
    bool contains_pipe = false;
    int cnt = 0;

    while (args[index] != NULL) 
    {
        // Check input: |
        if (strcmp(args[index], "|") == 0) 
        {
            split_index = index;
            contains_pipe = true;
        }
        index++;
    }
    
    if (!contains_pipe) 
    {
        return false;
    }
    
    // Copy arguments before split pipe position to child1_args[]
    for (index = 0; index < split_index; index++) 
    {
        child1_args[index] = strdup(args[index]);
    }
    child1_args[index++] = NULL;
        
    // Copy arguments after split pipe position to child2_args[]
    while (args[index] != NULL) 
    {
        child2_args[index - split_index - 1] = strdup(args[index]);
        index++;
    }
    child2_args[index - split_index - 1] = NULL;
    
    return true;
}

void child(char* args[], char* redirec_args[]) 
{
    int fd_out, fd_in;
    if (redirec_args[0] != NULL) 
    {
        // Redirecting Input
        if (strcmp(redirec_args[0], "<") == 0)
        {
            fd_in = open(redirec_args[1], O_RDONLY);
            if (fd_in == -1) 
            {
                printf("Redirect input failed\n");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);

            if (close(fd_in) == -1) 
            {
                printf("Closing input failed\n");
                exit(EXIT_FAILURE);
            }
        }
        // Redirecting Output
        else if (strcmp(redirec_args[0], ">") == 0) 
        {
            // Get file description
            fd_out = creat(redirec_args[1], S_IRWXU);
            if (fd_out == -1) 
            {
                printf("Redirect output failed\n");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            
            if (close(fd_out) == -1) 
            {
                printf("Closing output failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    // Execute in child process
    if (execvp(args[0], args) == -1) 
    {
        printf("Fail to execute command\n");
        exit(EXIT_FAILURE);
    }
}

void parent(pid_t child_pid, int wait)
{
    int status;
    printf("Parent <%d> - Child <%d>.\n", getpid(), child_pid);
    // Running concurrently
    if (wait == 0)
    {
        waitpid(child_pid, &status, 0);
    }
    // Parent wait child
    else
    {
        waitpid(child_pid, &status, WUNTRACED);
        if (WIFEXITED(status))
        {   
            printf("Child <%d> exited with status = %d.\n", child_pid, status);
        }
    }
    
}

void exec_with_pipe(char* child1_args[], char* child2_args[]) 
{
    int pipefd[2];
    if (pipe(pipefd) == -1) 
    {  
        //Create a pipe with 1 input and 1 output file descriptor
        //Index = 0 ==> read pipe, Index = 1 ==> write pipe
        printf("pipe() failed\n");
        exit(EXIT_FAILURE);
    }

    // Create 1st child   
    if (fork() == 0) 
    {
        // Redirect STDOUT to output part of pipe 
        dup2(pipefd[1], STDOUT_FILENO);       
        close(pipefd[0]);     
        close(pipefd[1]);      

        execvp(child1_args[0], child1_args);   
        printf("Fail to execute first command\n");
        exit(EXIT_FAILURE);
    }
    // Create 2nd child
    if (fork() == 0) 
    {
        // Redirect STDIN to input part of pipe
        dup2(pipefd[0], STDIN_FILENO);            
        close(pipefd[1]);      
        close(pipefd[0]);       

        execvp(child2_args[0], child2_args);   
        printf("Fail to execute second command\n");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    // Wait for child 1
    wait(0);   
    // Wait for child 2
    wait(0);   
}


int main()
{
    int should_run = 1;
    pid_t pid;
    int status = 0, count_history = 0, wait;
    char line[MAX_LENGTH];
    char *args[BUF_SIZE];
    char *redirec_args[2];
    char *child1_args[3], *child2_args[3];
    char *history[MAX_HISTORY];
    bool check_continue = false;

    for (int i = 0; i < MAX_HISTORY; i++) 
    {
        history[i] = (char*)malloc(MAX_COMMAND_NAME);
    }

    while (should_run) 
    {
        printf("osh>");
        fflush(stdout);

        readline(line);
        // Check input: exit
        if (strcmp(line, "exit") == 0) 
        {
            should_run = 0;
            continue;
        }
        // Check input: !!. history and update history
        check_continue = history_feature(history, count_history, line);
        if (check_continue == true)
        {
            continue;
        }
        // Check input: &. Run concurrently
        if (line[strlen(line) - 1] == '&') 
        {
            wait = 1;
            line[strlen(line) - 1] = '\0';
        }
        else 
        {
            wait = 0;
        }

        split_line(line, args);
        split_redirection(args, redirec_args);
        
        if (split_pipe(args, child1_args, child2_args)) 
        {
            exec_with_pipe(child1_args, child2_args);
            continue;
        }
        
        // Fork child process
        pid_t pid = fork();

        // Fork return twice on success: 0 - child process, > 0 - parent process
        switch (pid) 
        {
            case -1:
                printf("fork() failed!\n");
                exit(EXIT_FAILURE);
        
            case 0:     // In child process
                child(args, redirec_args);
                exit(EXIT_SUCCESS);
        
            default:    // In parent process
                parent(pid, wait);
        }
        
    }
    return 0;
}