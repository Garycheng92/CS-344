/*
	Harinder Gakhal
	CS 344 - Program 4
	otp_enc.c
	3/13/20
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

void main(int argc, char const *argv[]) {
	// Check if correct number of args are given and if the files exist
	if (argc != 4) {
		fprintf(stderr, "Please run command with three args: otp_enc [plaintext] [key] [port]\n");
		exit(0);
	}
	else if (access(argv[1], F_OK) == -1){
		fprintf(stderr, "\"%s\" Does not exist!\n", argv[1]);
		exit(0);
	}
	else if (access(argv[2], F_OK) == -1) {
		fprintf(stderr, "\"%s\" Does not exist!\n", argv[2]);
		exit(0);
	}

	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[100000];

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// open file1
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(argv[1], "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	read = getline(&line, &len, fp);
	char msg[strlen(line)];
	strcpy(msg, line);
	fclose(fp);

	//get file size
	fp = fopen(argv[1], "r");
	fseek(fp, 0L, SEEK_END);
	long int sizeofMSG = ftell(fp);
	fclose(fp);

	char sofMSG[100000];
	sprintf(sofMSG, "%ld", sizeofMSG);

	// Send size of first file to server
	charsWritten = send(socketFD, sofMSG, strlen(sofMSG), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(sofMSG)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	// if (buffer[0] != 'e'){
	// 	fprintf(stderr, "THE CLIENT IS CONNECTED TO THE WRONG SERVER!\n");
	// 	exit(2);
	// }

	// Send first file to server
	charsWritten = send(socketFD, msg, strlen(msg), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(msg)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	// open file2
	FILE * fp2;
	char * line2 = NULL;
	size_t len2 = 0;
	ssize_t read2;

	fp2 = fopen(argv[2], "r");
	if (fp2 == NULL)
		exit(EXIT_FAILURE);

	read2 = getline(&line2, &len2, fp2);
	char key[strlen(line2)];
	strcpy(key, line2);
	fclose(fp2);

	//get file size
	fp2 = fopen(argv[2], "r");
	fseek(fp2, 0L, SEEK_END);
	long int sizeofKEY = ftell(fp2);

	char sofKEY[100000];
	sprintf(sofKEY, "%ld", sizeofKEY);

	// Send size of second file to server
	charsWritten = send(socketFD, sofKEY, strlen(sofKEY), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(sofKEY)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	// Send second file to server
	charsWritten = send(socketFD, key, strlen(key), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(key)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	sizeofMSG -= strlen(buffer);
	printf("%s", buffer);

	// If message is not fully sent, receive message again
	while (sizeofMSG != 0) {
		if (strlen(buffer) == 0)
			break;
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("CLIENT: ERROR reading from socket\n");
		sizeofMSG -= strlen(buffer);
		printf("%s", buffer);
	}

	close(socketFD); // Close the socket
}