#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

/*Struct that represents a single room*/
typedef struct {
	char name[9];
	int connections[6];
	int numOfConnections;
	char type; /* can be s, m, or e */
} Room;

pthread_mutex_t mtx;

/*Prototypes used*/
char* getDir();
Room* buildMap(char* dir);
int findIndex(Room* roomList, char* str);
int startGame(Room* roomList);
int getStartRoom(Room* roomList);
void printPossibleConnections(Room* roomList, int);
int isConnected(Room* roomList, char* str, int currentRoom);
void printHistory(Room* roomList, int* arr, int count);
void* getTime();
void mutexThread();
void printTime();

int main(int argc, char const *argv[]) {
	return startGame(buildMap(getDir()));
}

/*Start game function will begin the game after it is given the room data structure*/
int startGame(Room* roomList) {
	/*Vars used:
		current room is the index of the current room the player is in
		exit lets the function know when to end the game
		askAgain will keep asking the user for an input until they give a valid result
		count keeps track of how many steps the user takes
	*/
	int currentRoom = getStartRoom(roomList), exit = 0, askAgain = 0, count = 0;
	int history[1000];
	char readIn[100], str[100];
	int valid = 0;

	/*Run the game until the exit flag is 1*/
	while (exit == 0) {
		/*Keep asking the user for an valid input*/
		while (askAgain == 0) {
			printf("CURRENT LOCATION: %s\n", roomList[currentRoom].name);	/*print current location*/
			printf("POSSIBLE CONNECTIONS: ");								/*print the possible connections*/
			printPossibleConnections(roomList, currentRoom);
			printf("WHERE TO? >");
			if (fgets(readIn, 100, stdin) == NULL) {						/*Ask user for input*/
				return -1;													/*If ctl-d is pressed, exit game*/
			}
			else
				sscanf(readIn, "%s", str);									/*Snip the newline off readIn*/

			if (strcmp(str, "time") == 0) {									/*Print time if user asks*/
				mutexThread();
				printTime();
			}
			else if (findIndex(roomList, str) == -1 || isConnected(roomList, str, currentRoom) == -1)	/*Check to see if the input is a valid room*/
				printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			else {
				askAgain = 1;								/*If input is valid, exit this while loop*/
				currentRoom = findIndex(roomList, str);		/*Update the current room*/
				history[count] = currentRoom;				/*Add the current room index to the users path*/	
				count++;									/*Increment the count*/
			}
			printf("\n");
		}
		if (roomList[currentRoom].type == 'e') {			/*If the current room is the end, print success and the useres path*/
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", count);
			printHistory(roomList, history, count);
			exit = 1;
		}
		else
			askAgain = 0;									/*Otherwise keep playing the game, and ask for another input*/
	}
	return 0;
}

/*getTime will put the current time and data into a file called currentTime.txt*/
void* getTime(){
	FILE* timeFile = fopen("currentTime.txt", "w+");		/*Make the file*/
	char buf[100];
	struct tm *sTm;
	time_t now = time(0);
	sTm = localtime(&now);
	strftime (buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", sTm);	/*Format current time*/
	fputs(buf, timeFile);									/*Put formatted time into the file*/
	fclose(timeFile);
}

/*printTime will read from a file - currentTime.txt - and print it out to the screen*/
void printTime(){
	FILE* timeFile = fopen("currentTime.txt", "r+");
	char buf[100];
	if(timeFile == NULL)
		perror("currentTime.txt DOES NOT EXIST!!\n");		/*If file DNE, print err*/
	else {
		fgets(buf, 100, timeFile);	/*Read from timeFile*/
		printf("\n%s\n", buf);		/*Print to screen*/
		fclose(timeFile);
	}
}

/*mutexThread will spwn new thread and lock/unlock accordingly*/
void mutexThread() {
	pthread_t threaded;					/*set thread*/
	pthread_mutex_init(&mtx, NULL);
	pthread_mutex_lock(&mtx);			/*Lock thread*/
	int tid = pthread_create(&threaded, NULL, getTime, NULL);	/*spawn new thread on getTime*/
	pthread_mutex_unlock(&mtx);			/*Un lock thread*/
	pthread_mutex_destroy(&mtx);		/*Used this website for help: http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html*/
	usleep(5000);
}

/*printHistory will print the path the user has taken during the game*/
void printHistory(Room* roomList, int* arr, int count) {
	int i;
	for (i = 0; i < count; ++i) {				/*Loop through history array and print the names of the rooms*/
		printf("%s\n", roomList[arr[i]].name);
	}
}

/*isConnected will return 1 if the string is a connected room to - else 0*/
int isConnected(Room* roomList, char* str, int currentRoom) {
	int i, lookingFor = findIndex(roomList, str);	/*init i and the index we are looking for*/
	for (i = 0; i < roomList[currentRoom].numOfConnections; ++i)
		if (roomList[currentRoom].connections[i] == lookingFor)
			return 1;
	return -1;	/*return -1 if we could not find the name*/
}

/*printPossibleConnections will print out all the connections to the current room*/
void printPossibleConnections(Room* roomList, int currentRoom) {
	int i;
	for (i = 0; i < roomList[currentRoom].numOfConnections; ++i) {			/*Loop through all the connected rooms and print the names*/
		printf("%s", roomList[roomList[currentRoom].connections[i]].name);	/*If we are not on the last room print , */
		if (i < roomList[currentRoom].numOfConnections-1)
			printf(", ");
	}
	printf(".\n");
}

/*getStartRoom will return the index of the starting room*/
int getStartRoom(Room* roomList) {
	int i;
	for (i = 0; i < 7; ++i)				/*Loop through the whole list*/
		if (roomList[i].type == 's')	/*Return the index*/
			return i;
	return -1;							/*If not found return -1*/
}

/*getDir will return the name of most recently made dir*/
char* getDir() {
	struct dirent *de;
	struct stat buf;
	DIR *dr = opendir(".");
	char *dirName;
	long newest = -1;

	while ((de = readdir(dr)) != NULL) {	/*go through each directory*/
		stat(de->d_name, &buf);
		if (buf.st_mtime >= newest && de->d_name[13] == '.') {	/*if the name is gakhalh.buildrooms.<PID> then check the mtime*/
			newest = buf.st_mtime;
			dirName = de->d_name;
		}
	}
	closedir(dr);
	return dirName;		/*Return the latest directory*/
}

/*buildMap will read from the files created from gakhal.buildrooms.c and put it into a data structure*/
Room* buildMap(char* dir) {
	struct dirent *de;
	char PATH[100], filePATH[500], line[1000], tmp[100];
	sprintf(PATH, "./%s/", dir);
	Room* roomList = malloc(7 * sizeof(Room));
	DIR *dr = opendir(PATH);

	/*Go through each file and assign the name, type and number of connections to the list of rooms*/
	int i = 0;
	while ((de = readdir(dr)) != NULL) {
		if (de->d_name[0] == 'R') {							/*If the file name starts with R*/
			sprintf(filePATH, "%s%s", PATH, de->d_name);
			FILE* fp = fopen(filePATH, "r");				/*Open the file*/
			while (fgets(line, 1000, fp) != NULL) {			/*Read each line of the file*/
				sscanf(line, "%*[^:]: %[^]%*[^\n]", tmp);	/*Snip everything off befre the ': '*/
				if (line[5] == 'N') {						/*If the line you are reading is ROOM NAME*/
					strcpy(roomList[i].name, tmp);			/*Put into struct*/
					roomList[i].numOfConnections = 0;		/*Set num of rooms to 0*/
				}
				else if (line[5] == 'T')					/*If the line you are reading is ROOM TYPE*/
					if (tmp[0] == 'S')
						roomList[i].type = 's';
					else if (tmp[0] == 'E')					/*Assign type*/
						roomList[i].type = 'e';
					else
						roomList[i].type = 'm';
			}
			fclose(fp);
			i++;
		}
	}

	/*Iterate through each file again, to assign the connections*/
	i = 0;
	dr = opendir(PATH);
	while ((de = readdir(dr)) != NULL) {
		if (de->d_name[0] == 'R') {
			sprintf(filePATH, "%s%s", PATH, de->d_name);
			FILE* fp = fopen(filePATH, "r");				/*Open each file*/
			while (fgets(line, 1000, fp) != NULL) {			/*Read each line*/
				sscanf(line, "%*[^:]: %[^]%*[^\n]", tmp);
				if (line[0] == 'C') {
					roomList[i].connections[roomList[i].numOfConnections] = findIndex(roomList, tmp);
					roomList[i].numOfConnections++;
				}
			}
			fclose(fp);
			i++;
		}
	}
	closedir(dr);
	return roomList;
}

/*findIndex will return the index of the room you are trying to search*/
int findIndex(Room* roomList, char* str) {
	int i;
	for (i = 0; i < 7; ++i)		/*Loop through each rooms*/
		if (strcmp(roomList[i].name, str) == 0)	/*If room name matches the string given*/
			return i;							/*Return the index*/
	return -1;									/*Else return -1*/
}
