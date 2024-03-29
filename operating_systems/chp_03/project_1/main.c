#include <stdlib.h>   // exit(); EXIT_FAILURE; EXIT_SUCCESS;
#include <stdio.h>    // printf(); fprintf(); stdout; stdin; fflsuh(); fgets(); perror();
#include <unistd.h>   // fork(); execvp();
#include <string.h>   // strcpy(); strcspn(); strcmp();
#include <sys/wait.h> // pid_t; WEXITSTATUS(); WIFEXITED(); wait();
#include <fcntl.h>    // open(); 0_WRONLY; O_TRUNC; O)CREAT;

// Typedefs, Macros & Personal Definitions & Includes
#define MAXLENCOMM 1024 /* The maximum command length */
#define MAX_ARG 128     /* The maximum argument length */
#define MAXHIST 64      /* The maximum saved history */
#define false 0
#define true 1
typedef _Bool bool;
typedef unsigned int uint;
#include "circularbuffer.c" // <-- My personal includes

int main()
{
    struct CircularBuffer commandHistory = {.size = 0, .head = 0};
    int should_run = true;      /* flag to determine when to exit program */
    bool should_wait = true;    /* <-- flag to determine if Child Process should wait To Finish */
    char redirectFile[MAX_ARG];

    while(should_run)
    {
	memset(redirectFile, 0, sizeof(redirectFile));
	printf("\033[;32m");        // <-- Create Green text
	fprintf(stdout, "osh> ");   // <-- Prompt User for Input
	fflush(stdout);             // <-- Flush I/O (Output) buffers
	printf("\033[;37m");        // <-- Revert back to White Text
	char string[MAXLENCOMM];
	char *arguments[MAX_ARG];
	char *pipeArguments[MAX_ARG];
	fgets(string, MAXLENCOMM, stdin); // <-- Read user input from stdIn
	string[strcspn(string, "\n")] = 0; // <-- Remove User Input NewLine Character "\n"
        
        // Detect "!!" and empty string "" inputs <-- Handle Accordingly
        uint prevIndex;
        if( string != NULL && !strcmp(string, "!!") ) {
            if (commandHistory.size == 0) {
                fprintf(stdout, "No commands in history.\n");
                fflush(stdout);
            }
            else if ( commandHistory.head == 0 ) {
                prevIndex = MAXHIST - 1;
                strcpy(string, commandHistory.history[prevIndex]);
                fprintf(stdout, "No commands in history.\n");
                fflush(stdout);
            }
            else {
                prevIndex = commandHistory.head - 1;
                strcpy(string, commandHistory.history[prevIndex]);
                fprintf(stdout, "No commands in history.\n");
                fflush(stdout);
            }
        }
        // If Valid Input, Add Entry To History List
        else if ( strcmp(string, "") ) {
            addEntry(&commandHistory, string);
        }

        // Tokenize Input String
        uint lastIndex = 0;
        char *token;
        token = strtok(string, " ");

        for(uint i = 0; token != NULL; i++) {
            arguments[i] = token;
            arguments[i + 1] = NULL; // <-- Insert "NULL" as last argument
            token = strtok(NULL, " ");
            lastIndex = i;
        }

        // Detect "exit" and "&" Inputs
        if( arguments[0] != NULL && !strcmp(arguments[0], "exit") ) { // <-- Exit if "exit" command
            should_run = false;
        }
        should_wait = true;
        if( arguments[lastIndex] != NULL && !strcmp(arguments[lastIndex], "&") ) {
            should_wait = false;
            arguments[lastIndex] = NULL;
        }

        // Fork() and Execute Command In Child Process
        pid_t parent;

        if( (parent = fork()) == -1 ) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

	// pid for in case a pipe "|" operator was present
	pid_t pipeParent;
	
	// Child Process
        if(!parent) {
	    bool redirectOutput = false;
	    bool redirectInput = false;
	    bool pipeFlagSet = false;	/* Flag to set when a pipe operator was found in the command */
	    int fd; /* File descriptor for re-direction file */
	    uint pipeIndex = 0; /* size of the pipe arguments array */
	    int pipeFd[2]; /* pipe file descriptors */

            if(should_run) {

		for(uint i = 0; i <= lastIndex; i++) {

		  if( redirectInput || redirectOutput ) {
		    strcpy(redirectFile, arguments[i]);// <-- Copy Re-direct argument to "redirectFile" variable
		    arguments[i] = NULL;	       // <-- Set (output) or (input) Re-direct string argument to NULL
		  }
		  else if ( pipeFlagSet ) {
		    pipeArguments[pipeIndex] = arguments[i];// <-- Copy all subsequent arguments after the "|" operator to a
		    arguments[i] = NULL;		    // <-- new argument array for piping between processes and set all
		    pipeIndex++;			    // <-- existing arguments after "|" in the original array to NULL
		  }
		  else {
		    if( !strcmp(arguments[i], ">") ) {
		      redirectOutput = true;		    // <-- Set Output Re-direct flag to TRUE
		      arguments[i] = NULL;		    // <-- set ">" argument string to NULL
		    }
		    else if( !strcmp(arguments[i], "<") ) {
		      redirectInput = true;		      // <-- Set Input Re-direct flag to TRUE 
		      arguments[i] = NULL;		      // <-- set "<" argument string to NULL
		    }
		    else if( !strcmp(arguments[i], "|") ) {
		      pipeFlagSet = true;		// <-- Set Pipe flag to TRUE
		      arguments[i] = NULL;		// <-- set "|" argument string to NULL
		    }
		  }
		}

		// If (Output) Re-direct flag has been set
		if(redirectOutput) {
		  /* Create output file, GET file descriptor */
		  fd = open(redirectFile, O_WRONLY| O_TRUNC | O_CREAT, 0666);
		  dup2(fd, STDOUT_FILENO);
		}

		// If (input) Re-direct flag has been set
		if(redirectInput) {
		  /* Read from input file, GET file descriptor */
		  fd = open(redirectFile, O_RDONLY);
		  dup2(fd, STDIN_FILENO);
		}

		// If ( | ) Pipe flag has been set
		if( pipeFlagSet ) {
		  
		  // Create pipe
		  if ( pipe(pipeFd) == -1 ) {
		    exit(EXIT_FAILURE);
		  }
		  
		  // Create another CHILD process for piping
		  if( (pipeParent = fork()) == -1 ) {
		      perror("fork");
		      exit(EXIT_FAILURE);
		  }

		  // <-- Pipe Child -->
		  if ( !pipeParent ) {
		    dup2(pipeFd[1], STDOUT_FILENO);
		    close(pipeFd[0]);
		    close(pipeFd[1]);

		    // Run the Command all after "|" pipe operator
		    int err = execvp(arguments[0], arguments);
		    if(err == -1) {
		      exit(EXIT_FAILURE); // Always use exit() in child processes
		    }
		  }

		  // <-- Pipe Parent -->
		  else {
		    dup2(pipeFd[0], STDIN_FILENO);
		    close(pipeFd[0]);
		    close(pipeFd[1]);
		  }
		}

		// IF piping operator found run everything after the "|" Command that has been input
		if (pipeFlagSet) {
		  int err = execvp(pipeArguments[0], pipeArguments);
		  if(err == -1) {
		    exit(EXIT_FAILURE); // Always use exit() in child processes
		  }
		}
		// ELSE Run the Command that has been input
		else {
		  int err = execvp(arguments[0], arguments);
		  if(err == -1) {
		    exit(EXIT_FAILURE); // Always use exit() in child processes
		  }
		}
            }

            exit(EXIT_SUCCESS); // Always use exit() in child processes
	}

	// Parent Process
        else {
            if(should_wait) { // <-- If "&" argument was NOT added to the END
                pid_t wStatus;
                wait(&wStatus);
                while( (wait(&wStatus)) > 0 ) { //<-- Parent waits for all the child processes
                    // If Program Finished Execution Normally i.e. (Without External SIG-Kill)
                    if(WIFEXITED(wStatus)) {
                        int statusCode = WEXITSTATUS(wStatus); // <-- Capture Exit Status
                        if(statusCode == 0) {
                            // Program Exited With A Status Code Of EXIT_SUCCESS
                        } else {
                            // Program Exited With A Status Code Of EXIT_FAILURE
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
