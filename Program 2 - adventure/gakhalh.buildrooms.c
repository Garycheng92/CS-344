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

int i, j;
/*const char* names[] = {"Crowther", "Dungeon", "PLUGH", "PLOVER", "twisty", "XYZZY", "Zork"};
*/const char* names[] = {"RoomA", "RoomB", "RoomC", "RoomD", "RoomE", "RoomF", "RoomG"};

void main(int argc, char const *argv[]) {
	buildStructs();
}

Room* buildStructs() {
	srand(time(0));

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

	while(isGraphValid(roomList) == 0) {
		int w;
		for (w = 0; w < 7; ++w) {
			int r = randNum(0, 6);
			while (connectionExists(roomList[w], r) == 1 || r == w || roomList[r].numOfConnections >= 6) {
				if (roomList[w].numOfConnections >= 6)
					break;
				printf("%s is already connected to %s\n", roomList[w].name, roomList[r].name);
				r = randNum(0, 6);
			}
			printf("[%d][%d] Connecting %s with %s: TC = %d\n", w, r, roomList[w].name, roomList[r].name, roomList[w].numOfConnections);
			roomList[w].connections[roomList[w].numOfConnections] = r;
			roomList[r].connections[roomList[r].numOfConnections] = w;
			roomList[w].numOfConnections++;
			roomList[r].numOfConnections++;

			if(isGraphValid(roomList) == 1)
				break;
		}
	}

	printRooms(roomList);

	return roomList;
}

/*Generate random numbers between low and high, inclusive*/
int randNum(int low, int high){
	return (rand() % (high - low + 1)) + low;
}

void printRooms(Room* roomList) {
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
	for (i = 0; i < 7; ++i) {
		if (roomList[i].numOfConnections <3 )
			return 0;
	}
	return 1;
}

int connectionExists (Room room, int r) {
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