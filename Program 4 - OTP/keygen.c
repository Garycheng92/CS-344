/*
	Harinder Gakhal
	3/3/20
	keygen.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int isInt(const char *str, int len);

void main(int argc, char const *argv[]) {
	// Random seed
	srand(time(NULL));
	const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWQYZ ";

	// Check to see if only 2 args are given, and if the second arg is an int
	if (argc != 2) {
		fprintf(stderr, "Please run command with one arg: keygen [keylength]\n");
		exit(0);
	}
	else if (isInt(argv[1], strlen(argv[1])) == -1) {
		fprintf(stderr, "Please enter an integer for the second argument.\n");
		exit(0);
	}

	//Generate random chars
	int num = atoi(argv[1]);
	for (int i = 0; i < num; ++i) {
		int r = rand() % 27;
		printf("%c", chars[r]);
	}
	printf("\n");
}

// isInt will check if the second arg is an integer
int isInt(const char *str, int len) {
	for (int i = 0; i < len; ++i)
		if (str[i] > 57 || str[i] < 48)
			return -1;
	return 0;
}
