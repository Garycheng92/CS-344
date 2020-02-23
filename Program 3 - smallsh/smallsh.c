#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

void main(int argc, char const *argv[]) {
	char readIn[2048], *commandList[2048], str[512], pwd[100];	/*Init variables*/

	while (1) {											/*start shell*/
		printf(": ");
		fflush(stdout);									/*flush stdin*/
		fgets(readIn, 2048, stdin);
		int count = 0;

		char *token = strtok(readIn, " ");
		while( token != NULL ) {
			sscanf(token, "%s", str);
			commandList[count++] = strdup(str);
			token = strtok(NULL, " ");
		}
		commandList[count++] = NULL;

		if (strcmp(commandList[0], "exit") == 0)
			exit(0);
		else if (strcmp(commandList[0], "cd") == 0) {
			if (count == 1)
				chdir(getenv("HOME"));
			else
				chdir(commandList[1]);
		}
		else if (strcmp(commandList[0], "status") == 0) {
			if(WIFEXITED(0)){
				printf("exit value %i\n", WEXITSTATUS(0));
			}
			else{
				printf("terminated by signal %i\n", 0);
			}
		}
		else {
			pid_t spawnPID = -5;
			int childExitStatus = -5;

			spawnPID = fork();
			switch (spawnPID) {
				case -1: { 
					perror("Hull Breach!\n");
					_Exit(1); break;
				}
				case 0: {
					if(execvp(commandList[0], commandList)){
						printf("smallsh: Sorry, command \"%s\" is not a valid command\n", commandList[0]);
						fflush(stdout);
						_Exit(1);
					}
					_Exit(2);
				}
				default: {
					_Exit(0);
				}
			}
		}
	}
}