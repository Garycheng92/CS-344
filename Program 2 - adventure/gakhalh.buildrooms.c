#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	char name[9];
	int connections[6];
	int numOfConnections;
	char type; /* can be s, m, or e */
} Room;

int makeDir();
int makeFiles(char*);
Room* buildStructs();
int randNum(int lower, int upper);
void printRooms(Room* roomList);
int isGraphValid (Room* roomList);
int connectionExists (Room room, int r);

const char* names[] = {"Crowther", "Dungeon", "PLUGH", "PLOVER", "twisty", "XYZZY", "Zork"};

void main(int argc, char const *argv[]) {
	buildStructs();
}

Room* buildStructs() {
	srand(time(0));
	int i;

	Room* roomList = malloc(7 * sizeof(Room));

	for (i = 0; i < 7; ++i) {
		strcpy(roomList[i].name, names[i]);
		roomList[i].numOfConnections = 0;
	}

	int num = randNum(0,6);
	roomList[num].type = 's';

	int newNum = randNum(0,6);
	while (newNum == num) {
		newNum = randNum(0,6);
	}
	roomList[newNum].type = 'e';

	for (i = 0; i < 7; ++i) {
		if (roomList[i].type != 's' && roomList[i].type != 'e')
			roomList[i].type = 'm';
	}

	roomList[0].connections[roomList[0].numOfConnections] = 6;
	roomList[6].connections[roomList[6].numOfConnections] = 0;
	roomList[0].numOfConnections++;
	roomList[6].numOfConnections++;

	printRooms(roomList);

	printf("%d\n", connectionExists(roomList[0], 5));

	return roomList;
}

int randNum(int low, int high){
	return (rand() % (high - low + 1)) + low;
}

void printRooms(Room* roomList) {
	int i, j;
	for (i = 0; i < 7; ++i) {
		printf("Name: %s\nType: %c\nnoC: %d\n", roomList[i].name, roomList[i].type, roomList[i].numOfConnections);
		printf("Connections: ");
		for (j = 0; j < roomList[i].numOfConnections; ++j) {
			printf("%s, ", roomList[roomList[i].connections[j]].name); 
		}
		printf("\n\n");
	}
}

int isGraphValid (Room* roomList) {
	int i;
	for (i = 0; i < 7; ++i) {
		if (roomList[i].numOfConnections <3 )
			return 0;
	}
	return 1;
}

int connectionExists (Room room, int r) {
	int i, j;
	for (i = 0; i < room.numOfConnections; ++i) {
		if (room.connections[i] == r)
			return 1;
	}
	return 0;
}

/*int makeDir() {
	char dirName[100] = "gakhalh.buildrooms.";
	char PID[5];
	sprintf(PID, "%d", getpid());

	strcat(dirName, PID);
	printf("%s\n", dirName);
	int check = mkdir(dirName);

	if (!check) {
		makeFiles(dirName);
		return 1;
	}
	else
		return 0;
}

int makeFiles(char* dirName) {
	char PATH[100];
	struct Room* roomList[7];
	strcpy(roomList[0]->name, "Crowther");
	strcpy(roomList[1]->name, "Dungeon");
	strcpy(roomList[2]->name, "PLUGH");
	strcpy(roomList[3]->name, "PLOVER");
	strcpy(roomList[4]->name, "twisty");
	strcpy(roomList[5]->name, "XYZZY");
	strcpy(roomList[6]->name, "Zork");

	int i;
	for (i = 0; i < 7; ++i)
	{
		sprintf(PATH, "./%s/%s_room", dirName, roomList[i]->name);
		FILE *fp = fopen(PATH , "w");
		fclose(fp);
	}
}*/