#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>


/*
*  Assignment : #2
*  Write a C program called time.c that determines the amount of time necessary
*  to run a command from the command line. This program will be run as "./time <command>"
*  and will report the amount of elapsed time to run the specified command. This will involve
*  using fork() and exec() functions, as well as the gettimeofday() function to determine
*  the elapsed time. It will also require the use of two different IPC mechanisms.
* -----------------------------------------------------------------------------------------------------
*  The general strategy is to fork a child process that will execute the specified command.
*  However, before the child executes the command, it will record a timestamp of the current
*  time (which we term "starting time"). The parent process will wait for the child process to
*  terminate. Once the child terminates, the parent will record the current timestamp for the
*  ending time. The difference between the starting and ending times represents the elapsed time
*  to execute the command. The example output below reports the amount of time to run the command ls:
* -----------------------------------------------------------------------------------------------------
*  The first version will have the child process write the starting time to a region of
*  shared memory before it calls exec(). After the child process terminates, the parent
*  will read the starting time from shared memory. Refer to Section POSIX shared memory
*  for details using POSIX shared memory. In that section, there are separate programs for
*  the producer and consumer. As the solution to this problem requires only a single program,
*  the region of shared memory can be established before the child process is forked, allowing
*  both the parent and child processes access to the region of shared memory.
*/

int main(int argc, char **argv)
{
    // Create timeval struct that gets the Unix Epoch time value which is
    // accurate to the nearest microsecond but also has a range of years.
    // Both "PARENT" and "CHILD" will have this struct in their memory
    struct timeval current_time;

    // Create child process and identify each
    // parent and child process with their unique pid
    pid_t pid = fork();

    // If "CHILD" process
    if (pid == 0)
    {
        gettimeofday(&current_time, NULL);
        printf("\nChild process : \n  seconds : %ld\n  micro seconds : %ld\n\n", current_time.tv_sec, current_time.tv_usec);


        // char *binaryPath = "/bin/ls";
        // char *args[] = {binaryPath, "-lh", "/home", NULL};
        // execv(binaryPath, args);
        exit(17);
    }
    // else "PARENT" process
    else
    {
        int child_status;
        waitpid(pid, &child_status, 0);
        printf("\nI will always wait for the child to finish before printing...\n\n");
    }
    
    
    return 0;
}
