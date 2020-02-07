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

/* Prototypes for all the functions */
int makeFiles(Room*);
Room* buildStructs();
int randNum(int lower, int upper);
void printRooms(Room* roomList);
int isGraphValid (Room* roomList);
int connectionExists (Room room, int r);
int* pickNames(char** names);
int isInList(int* arr, int num, int len);

/* List of possible room names */
char* names[] = {"RoomA", "RoomB", "RoomC", "RoomD", "RoomE", "RoomF", "RoomG", "RoomH", "RoomI", "RoomJ"};

void main(int argc, char const *argv[]) {
	Room* myRooms = buildStructs();
	makeFiles(myRooms);
	free(myRooms);
}

/* buildStructs() will build out the room nodes and connect them accordingly. */
Room* buildStructs() {
	srand(time(0));
	int i;

	Room* roomList = malloc(7 * sizeof(Room));
	int* randNames = pickNames(names);						/* Generate random indices */

	for (i = 0; i < 7; ++i) {								/*Assign room names and set number of connections to 0*/
		strcpy(roomList[i].name, names[randNames[i]]);
		roomList[i].numOfConnections = 0;
	}
	free(randNames);

	/*Set start and end room*/
	int num = randNum(0,6);
	roomList[num].type = 's';

	int newNum = randNum(0,6);
	while (newNum == num) {
		newNum = randNum(0,6);
	}
	roomList[newNum].type = 'e';

	/*Set the rest to mid room*/
	for (i = 0; i < 7; ++i) {
		if (roomList[i].type != 's' && roomList[i].type != 'e')
			roomList[i].type = 'm';
	}

	/*While the graph is not valid, keep connecting rooms*/
	while(isGraphValid(roomList) == 0) {
		int w;
		for (w = 0; w < 7; ++w) {
			int r = randNum(0, 6);
			while (connectionExists(roomList[w], r) == 1 || r == w || roomList[r].numOfConnections >= 6) {
				if (roomList[w].numOfConnections >= 6)
					break;
				r = randNum(0, 6);
			}
			roomList[w].connections[roomList[w].numOfConnections] = r;
			roomList[r].connections[roomList[r].numOfConnections] = w;
			roomList[w].numOfConnections++;
			roomList[r].numOfConnections++;

			if(isGraphValid(roomList) == 1)
				break;
		}
	}

	return roomList;
}

/* Generate random numbers between low and high, inclusive */
int randNum(int low, int high){
	return (rand() % (high - low + 1)) + low;
}

void printRooms(Room* roomList) {
	int i,j;
	for (i = 0; i < 7; ++i) {
		printf("ROOM NAME: %s\n", roomList[i].name);
		for (j = 0; j < roomList[i].numOfConnections; ++j) {
			printf("CONNECTION %d: %s\n", j+1, roomList[roomList[i].connections[j]].name); 
		}
		if (roomList[i].type == 's')
			printf("ROOM TYPE: START_ROOM\n");
		else if (roomList[i].type == 'e')
			printf("ROOM TYPE: END_ROOM\n");
		else
			printf("ROOM TYPE: MID_ROOM\n");
		printf("\n");
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
	int i;
	for (i = 0; i < room.numOfConnections; ++i) {
		if (room.connections[i] == r)
			return 1;
	}
	return 0;
}

int* pickNames(char** names){
	int l;

	int* numList = malloc(7 * sizeof(int));
	for (l = 0; l<7; ++l) {
		numList[l] = -1;
	}

	int num = randNum(0,9);
	for (l = 0; l < 7; ++l) {
		while (isInList(numList, num, 7) == 1) {
			num = randNum(0,9);
		}
		numList[l] = num;
	}
	return numList;
}

int isInList(int* arr, int num, int len){
	int c;
	for (c = 0; c < len; ++c) {
		if (num == arr[c])
			return 1;
	}
	return 0;
}

int makeFiles(Room* roomList) {
	char dirName[100] = "gakhalh.rooms.";
	char PID[20];
	sprintf(PID, "%d", getpid());
	strcat(dirName, PID);
	int check = mkdir(dirName, 0660);

	if (!check) {
		char PATH[100];
		int i;
		for (i = 0; i < 7; ++i) {
			sprintf(PATH, "./%s/%s_room", dirName, roomList[i].name);
			FILE *fp = fopen(PATH,  "w+");
			fprintf(fp, "ROOM NAME: %s\n", roomList[i].name);
			int j;
			for (j = 0; j < roomList[i].numOfConnections; ++j) {
				fprintf(fp, "CONNECTION %d: %s\n", j+1, roomList[roomList[i].connections[j]].name); 
			}
			if (roomList[i].type == 's')
				fprintf(fp, "ROOM TYPE: START_ROOM\n");
			else if (roomList[i].type == 'e')
				fprintf(fp, "ROOM TYPE: END_ROOM\n");
			else
				fprintf(fp, "ROOM TYPE: MID_ROOM\n");
			fclose(fp);
		}
		return 1;
	}
	else
		return 0;
}
