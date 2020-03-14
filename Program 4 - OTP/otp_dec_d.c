/*
	Harinder Gakhal
	CS 344 - Program 4
	otp_dec_d.c
	3/13/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// Prototypes
char* decryptMessage(char* msg, char *key);
char* encryptMessage(char* msg, char *key);
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void main(int argc, char const *argv[]) {
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	while (1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "FORK ERROR\n");
			exit(1);
		}

		if (pid == 0) {
			// Get the message from the client and display it
			memset(buffer, '\0', 256);
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");

			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");

			// open file1
			FILE * fp;
			char * line = NULL;
			size_t len = 0;
			ssize_t read;

			fp = fopen(buffer, "r");
			if (fp == NULL)
				exit(EXIT_FAILURE);

			read = getline(&line, &len, fp);
			char msg[strlen(line)];
			strcpy(msg, line);

			// Get the message from the client and display it
			memset(buffer, '\0', 256);
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");

			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");

			// open file2
			FILE * fp2;
			char * line2 = NULL;
			size_t len2 = 0;
			ssize_t read2;

			fp2 = fopen(buffer, "r");
			if (fp2 == NULL)
				exit(EXIT_FAILURE);

			read2 = getline(&line2, &len2, fp2);
			char key[strlen(line2)];
			strcpy(key, line2);
			
			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, decryptMessage(msg, key), strlen(line), 0); // Send success back
			if (charsRead < 0) error("ERROR writing to socket");

			fclose(fp);
			if (line)
				free(line);

			fclose(fp2);
			if (line2)
				free(line2);
		}
		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
}

char* decryptMessage(char* msg, char *key) {
	// Throw error if the key is too short
	if (strlen(key) < strlen(msg)) {
		fprintf(stderr, "Error: key is too short!\n");
		exit(1);
	}

	char *encryptMessage = malloc (sizeof (char) * strlen(msg)), emsg[strlen(msg)-1];
	// character bank
	const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	// for each character in the message, subtract the key then mod 27 (if the difference is < 0, add 27 to it)
	for (int i = 0; i < strlen(msg)-1; ++i) {
		if ((msg[i] < 65 || msg[i] > 90) && msg[i] != ' '){
			fprintf(stderr, "error: input contains bad characters");
			exit(1);
		}
		else if (msg[i] == ' ' && (key[i] == ' ')){
			if ((msg[i] - 6) - (key[i] - 6) < 0)
				emsg[i] = ("%c", chars[((((msg[i] - 6) - (key[i] - 6)) + 27) % 27)]);
			else
				emsg[i] = ("%c", chars[((msg[i] - 6) - (key[i] - 6)) % 27]);
		}
		else if (msg[i] == ' ' && key[i] != ' ') {
			if ((msg[i] - 65) - (key[i] - 6) < 0)
				emsg[i] = ("%c", chars[((((msg[i] - 6) - (key[i] - 65)) + 27) % 27)]);
			else
				emsg[i] = ("%c", chars[((msg[i] - 6) - (key[i] - 65)) % 27]);
		}
		else if (key[i] == ' ' && msg[i] != ' ') {
			if ((msg[i] - 65) - (key[i] - 6) < 0)
				emsg[i] = ("%c", chars[((((msg[i] - 65) - (key[i] - 6) + 27)) % 27)]);
			else
				emsg[i] = ("%c", chars[((msg[i] - 65) - (key[i] - 6)) % 27]);
		}
		else {
			if ((msg[i] - 65) - (key[i] - 65) < 0)
				emsg[i] = ("%c", chars[((((msg[i] - 65) - (key[i] - 65) + 27)) % 27)]);
			else
				emsg[i] = ("%c", chars[((msg[i] - 65) - (key[i] - 65)) % 27]);
		}
	}
	emsg[strlen(msg)-1] = '\n';
	emsg[strlen(msg)] = '\0';

	// Put in returnable variable
	strcpy(encryptMessage, emsg);

	return encryptMessage;
}
