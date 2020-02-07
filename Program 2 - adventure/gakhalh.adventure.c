#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char name[9];
	int connections[6];
	int numOfConnections;
	char type; /* can be s, m, or e */
} Room;

char* getDir();
Room* buildMap(char* dir);
void printRooms(Room* roomList);
int findIndex(Room* roomList, char* str);
void startGame(Room* roomList);
int getStartRoom(Room* roomList);
void printPossibleConnections(Room* roomList, int);
int isConnected(Room* roomList, char* str, int currentRoom);
void printHistory(Room* roomList, int* arr, int count);

int main(int argc, char const *argv[]) {
	startGame(buildMap(getDir()));
}

void startGame(Room* roomList) {
	int currentRoom = getStartRoom(roomList), exit = 0, askAgain = 0, count = 0;
	int history[1000];
	char readIn[100], str[100];
	int valid = 0;

	while (exit == 0) {
		while (askAgain == 0) {
			printf("CURRENT LOCATION: %s\n", roomList[currentRoom].name);
			printf("POSSIBLE CONNECTIONS: ");
			printPossibleConnections(roomList, currentRoom);
			printf("WHERE TO? >");
			fgets(readIn, 100, stdin);
			sscanf(readIn, "%s", str);

			if (findIndex(roomList, str) == -1 || isConnected(roomList, str, currentRoom) == -1)
				printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			else {
				askAgain = 1;
				currentRoom = findIndex(roomList, str);
				history[count] = currentRoom;
				count++;
				printf("\n");
			}
		}
		if (roomList[currentRoom].type == 'e') {
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", count);
			printHistory(roomList, history, count);
			exit = 1;
		}
		else
			askAgain = 0;
	}
}

void printHistory(Room* roomList, int* arr, int count) {
	int i;
	for (i = 0; i < count; ++i) {
		printf("%s\n", roomList[arr[i]].name);
	}
}

int isConnected(Room* roomList, char* str, int currentRoom) {
	int i, lookingFor = findIndex(roomList, str);
	for (i = 0; i < roomList[currentRoom].numOfConnections; ++i)
		if (roomList[currentRoom].connections[i] == lookingFor)
			return 1;
	return -1;
}

void printPossibleConnections(Room* roomList, int currentRoom) {
	char str[200];
	int i;
	for (i = 0; i < roomList[currentRoom].numOfConnections; ++i) {
		printf("%s", roomList[roomList[currentRoom].connections[i]].name);
		if (i < roomList[currentRoom].numOfConnections-1)
			printf(", ");
	}
	printf(".\n");
}

int getStartRoom(Room* roomList) {
	int i;
	for (i = 0; i < 7; ++i)
		if (roomList[i].type == 's')
			return i;
	return -1;
}

char* getDir() {
	struct dirent *de;
	struct stat buf;
	DIR *dr = opendir(".");
	char *dirName;
	long newest = -1;

	while ((de = readdir(dr)) != NULL) {
		stat(de->d_name, &buf);
		if (buf.st_mtime >= newest && de->d_name[13] == '.') {
			newest = buf.st_mtime;
			dirName = de->d_name;
		}
	}
	closedir(dr);
	return dirName;
}

Room* buildMap(char* dir) {
	struct dirent *de;
	char PATH[100], filePATH[500], line[1000], tmp[100];
	sprintf(PATH, "./%s/", dir);
	Room* roomList = malloc(7 * sizeof(Room));
	DIR *dr = opendir(PATH);

	int i = 0;
	while ((de = readdir(dr)) != NULL) {
		if (de->d_name[0] == 'R') {
			sprintf(filePATH, "%s%s", PATH, de->d_name);
			FILE* fp = fopen(filePATH, "r");
			while (fgets(line, 1000, fp) != NULL) {
				sscanf(line, "%*[^:]: %[^]%*[^\n]", tmp);
				if (line[5] == 'N') {
					strcpy(roomList[i].name, tmp);
					roomList[i].numOfConnections = 0;
				}
				else if (line[5] == 'T')
					if (tmp[0] == 'S')
						roomList[i].type = 's';
					else if (tmp[0] == 'E')
						roomList[i].type = 'e';
					else
						roomList[i].type = 'm';
			}
			fclose(fp);
			i++;
		}
	}

	i = 0;
	dr = opendir(PATH);
	while ((de = readdir(dr)) != NULL) {
		if (de->d_name[0] == 'R') {
			sprintf(filePATH, "%s%s", PATH, de->d_name);
			FILE* fp = fopen(filePATH, "r");
			while (fgets(line, 1000, fp) != NULL) {
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

int findIndex(Room* roomList, char* str) {
	int i;
	for (i = 0; i < 7; ++i)
		if (strcmp(roomList[i].name, str) == 0)
			return i;
	return -1;
}

void printRooms(Room* roomList) {
	int i,j;
	for (i = 0; i < 7; ++i) {
		printf("ROOM NAME: %s\n", roomList[i].name);
		for (j = 0; j < roomList[i].numOfConnections; ++j)
			printf("CONNECTION %d: %s\n", j+1, roomList[roomList[i].connections[j]].name); 
		if (roomList[i].type == 's')
			printf("ROOM TYPE: START_ROOM\n");
		else if (roomList[i].type == 'e')
			printf("ROOM TYPE: END_ROOM\n");
		else
			printf("ROOM TYPE: MID_ROOM\n");
		printf("\n");
	}
}