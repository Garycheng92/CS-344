/*/*
	Harinder Gakhal
	CS 344 - Program 4
	otp_enc_d.c
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
char* encryptMessage(char* msg, char *key);
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

void main(int argc, char const *argv[]) {
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[1000000];
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

	while(1) {
		char msg[1000000] = "", key[1000000] = "";
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");

		// For every new connection
		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "FORK ERROR\n");
			exit(1);
		}

		if (pid == 0) {
			// Get the message from the client
			memset(buffer, '\0', 1000000);
			charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
			if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "s", 1, 0); // Send success back
			if (charsRead < 0) fprintf(stderr, "ERROR writing to socket");

			int sizeofMSG = atoi(buffer);

			// Get the message from the client and display it
			memset(buffer, '\0', 1000000);
			charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
			if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
			sizeofMSG -= strlen(buffer);
			strcat(msg, buffer);

			// If message is not fully sent receive message again
			while (sizeofMSG != 0) {
				if (strlen(buffer) == 0)
					break;
				memset(buffer, '\0', 1000000);
				charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
				if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
				sizeofMSG -= strlen(buffer);
				strcat(msg, buffer);
			}
			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your messageeee", 39, 0); // Send success back
			if (charsRead < 0) fprintf(stderr, "ERROR writing to socket");

			// Get the message from the client
			memset(buffer, '\0', 1000000);
			charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
			if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) fprintf(stderr, "ERROR writing to socket");

			int sizeofKEY = atoi(buffer);

			// Get the message from the client and display it
			memset(buffer, '\0', 1000000);
			charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
			if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
			sizeofKEY -= strlen(buffer);
			strcat(key, buffer);

			// If message is not fully sent receive message again
			while (sizeofKEY != 0) {
				if (strlen(buffer) == 0)
					break;
				memset(buffer, '\0', 1000000);
				charsRead = recv(establishedConnectionFD, buffer, 999999, 0); // Read the client's message from the socket
				if (charsRead < 0) fprintf(stderr, "ERROR reading from socket");
				sizeofKEY -= strlen(buffer);
				strcat(key, buffer);
			}
			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
			if (charsRead < 0) fprintf(stderr, "ERROR writing to socket");

			// Send a Success message back to the client
			charsRead = send(establishedConnectionFD, encryptMessage(msg, key), strlen(msg), 0); // Send success back
			if (charsRead < 0) fprintf(stderr, "ERROR writing to socket");
		}
		close(establishedConnectionFD); // Close the existing socket which is connected to the client
	}
	close(listenSocketFD); // Close the listening socket
}

char* encryptMessage(char* msg, char *key) {
	// Check if key is shorter than message
	if (strlen(key) < strlen(msg)) {
		fprintf(stderr, "Error: key is too short!\n");
		exit(1);
	}

	char *encryptMessage = malloc (sizeof (char) * strlen(msg)), emsg[strlen(msg)-1];
	// Letter bank
	const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	// Add key to msg then mod by 27
	for (int i = 0; i < strlen(msg)-1; ++i) {
		if ((msg[i] < 65 || msg[i] > 90) && msg[i] != ' '){
			fprintf(stderr, "error: input contains bad characters");
			exit(1);
		}
		else if (msg[i] == ' ' && (key[i] == ' '))
			emsg[i] = ("%c", chars[((msg[i] - 6) + (key[i] - 6)) % 27]);
		else if (msg[i] == ' ')
			emsg[i] = ("%c", chars[((msg[i] - 6) + (key[i] - 65)) % 27]);
		else if (key[i] == ' ')
			emsg[i] = ("%c", chars[((msg[i] - 65) + (key[i] - 6)) % 27]);
		else
			emsg[i] = ("%c", chars[((msg[i] - 65) + (key[i] - 65)) % 27]);
	}
	emsg[strlen(msg)-1] = '\n';
	emsg[strlen(msg)] = '\0';

	// put message into returnable variable
	strcpy(encryptMessage, emsg);

	return encryptMessage;
}