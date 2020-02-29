/*
	Harinder Gakhal
	CS 344
	Program 3 - smallsh
	2/26/20
*/

// libraries used
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

// prototypes
void catchSIGINT(int signo);
void catchSIGTSTP(int signal);
char *findAndReplace(char *s, int PID);

int fgONLY = 0;

void catchSIGINT(int signo) {					/*catch ctrl-c*/
	char* message = "\nCaught SIGINT.\n";
	write(STDOUT_FILENO, message, 38);
}

void catchSIGTSTP(int signal) {					/*catch ctrl-z*/
	if(fgONLY == 0) {
		char* message = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, 25);
		fgONLY = 1;
	}
	else {
		char* message = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, message, 25);
		fgONLY = 0;
	}
}

void main(int argc, char const *argv[]) {
	char readIn[2048], *commandList[2048], str[512], pwd[100];	/*Init variables*/
	int childExitStatus = -5, bg = 0, pListCount = 0;
	pid_t spawnPID = -5;
	struct sigaction action = {0}, action2 = {0};

	action.sa_handler = SIG_IGN;			/*set up signal handler for SIGINT*/
	action.sa_flags = 0;					/*ctrl-c disabled by default*/
	sigfillset(&(action.sa_mask));
	sigaction(SIGINT, &action, NULL);
	
	action2.sa_handler = catchSIGTSTP;		/*set up signal handler for SIGINT*/
	action2.sa_flags = 0;
	sigfillset(&(action2.sa_mask));
	sigaction(SIGTSTP, &action2, NULL);

	while (1) {													/*start shell*/
		printf(": ");
		fflush(stdout);											/*flush stdin*/
		strcpy(readIn, "\n");
		fgets(readIn, 2048, stdin);								/*Take in user input*/
		int count = 0;
		char fileIN[100] = {0}, fileOUT[100] = {0};				/*Input, output file names*/

		char *token = strtok(readIn, " ");
		while( token != NULL ) {								/*Iterate through each word in the line and parse it*/
			if (strcmp(token, "<") == 0) {
				token = strtok(NULL, " ");						/*File input*/
				sscanf(token, "%s", fileIN);
				token = strtok(NULL, " ");
			}
			else if (strcmp(token, ">") == 0) {					/*File output */
				token = strtok(NULL, " ");
				sscanf(token, "%s", fileOUT);
				token = strtok(NULL, " ");
			}
			else if (strstr(token, "$$") != NULL) {				/*Replace the $$ token with the pid*/
				sscanf(token, "%s", str);
				commandList[count++] = strdup(findAndReplace(str, getpid()));
				token = strtok(NULL, " ");
			}
			else {
				sscanf(token, "%s", str);						/*All other tokens go into the command list*/
				commandList[count++] = strdup(str);
				token = strtok(NULL, " ");
			}
		}

		if (readIn[0] == '\n')									/*If user typed in nothing, set command list to null*/
			commandList[0] = NULL;

		if (commandList[0] != NULL && commandList[0][0] != '#'){	/*If not comment or empty*/
			if(strcmp(commandList[count-1], "&") == 0) {			/*Check to see if it should be a background process*/
				commandList[count-1] = NULL;						/*Remove & from the command list*/
				bg = 1;
			}
			else {
				commandList[count++] = NULL;
				bg = 0;
			}

			if (strcmp(commandList[0], "exit") == 0)				/*Check to see if user typed exit*/
				exit(0);
			else if (strcmp(commandList[0], "cd") == 0) {			/*Check to see if user typed cd*/
				if (count-1 == 1)
					chdir(getenv("HOME"));
				else
					chdir(commandList[1]);
			}
			else if (strcmp(commandList[0], "status") == 0) {		/*Return the status of the last command is user typed status*/
				if(WIFEXITED(childExitStatus))
					printf("exit value %i\n", WEXITSTATUS(childExitStatus));
				else
					printf("terminated by signal %i\n", childExitStatus);
			}
			else {
				spawnPID = fork();									/*Fork the command if it is not recognized*/
				switch (spawnPID) {
					case -1: {
						perror("Hull Breach!\n");					/*Fork failure*/
						_Exit(1); break;
					}
					case 0: {
						if (bg == 0) {								/*Check to see if it is background process, and enable ctr-c*/
							action.sa_handler = SIG_DFL;
							sigaction(SIGINT, &action, NULL);
						}

						if(fileIN[0] != 0){							/*If file input it given*/
							int inDisc = open(fileIN, O_RDONLY);
							if (inDisc == -1) {						/*Error if file does not exist*/
								printf("FILE DNE: %s\n", fileIN);
								fflush(stdout);
								_Exit(1);
							}
							if(dup2(inDisc, 0) == -1) {				/*Print error if dup2 failed*/
								perror("dup2 error");
								_Exit(1);
							}
							close(inDisc);
						}

						if(fileOUT[0] != 0) {						/*Check to see if file output is given*/
							int outDisc = open(fileOUT, O_WRONLY | O_CREAT | O_TRUNC, 0777);
							if(outDisc == -1) {
								printf("smallsh: cannot open %s\n", fileOUT);
								fflush(stdout);
								_Exit(1);
							}
							if(dup2(outDisc, 1) == -1){				/*Print error if dup2 failed*/
								perror("dup2");
								_Exit(1);
							}
							close(outDisc);
						}

						if(execvp(commandList[0], commandList) < 0){	/*Run command list in terminal*/
							printf("Sorry, command \"%s\" is not a valid command\n", commandList[0]);
							fflush(stdout);
							_Exit(1);
						}
						break;
					}
					default: {
						if (bg == 0 || fgONLY == 1)							/*If foreground process, wait for the process to finish*/
							waitpid(spawnPID, &childExitStatus, 0);
						else if (bg == 1) {
							printf("background pid is %d\n", spawnPID);		/*If background process, run in the background with WNOGANG*/
							fflush(stdout);
						}
					}
				}
			}
			spawnPID = waitpid(-1, &childExitStatus, WNOHANG);				/*WNOHANG process*/
			while(spawnPID > 0){											/*Print status when it has been completed*/
				printf("background process, %i, is done: ", spawnPID);
				fflush(stdout);
				if(WIFEXITED(childExitStatus)) {
					printf("exit value %i\n", WEXITSTATUS(childExitStatus));
					fflush(stdout);
				}
				else {
					printf("terminated by signal %i\n", childExitStatus);
					fflush(stdout);
				}
				spawnPID = waitpid(-1, &childExitStatus, WNOHANG);
			}	
		}
	}
}

// Find and replace function used to replace the $$ token
char *findAndReplace(char *str, int PID) {
	char newW[100], *result;	//put PID into string
	sprintf(newW, "%d", PID);
	int i, count = 0, newWlen = strlen(newW);

	for (i = 0; str[i] != '\0'; i++) {
		if (strstr(&str[i], "$$") == &str[i]) {
			count++; 
			i++;
		}
	}

	/*walk through the word, and replace every instance of $$*/
	result = (char *)malloc(i + count * (newWlen - 2) + 1);

	i = 0;
	while (*str) {
		if (strstr(str, "$$") == str) { 
			strcpy(&result[i], newW); 
			i += newWlen; 
			str += 2; 
		}
		else
			result[i++] = *str++; 
	}

	result[i] = '\0';
	return result;
}