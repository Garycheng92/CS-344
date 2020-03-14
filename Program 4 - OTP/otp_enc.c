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

	// ---------------------------------------------------------------------------------------------

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

	// Send first file to server
	charsWritten = send(socketFD, argv[1], strlen(argv[1]), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(argv[1])) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	// printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	// Send second file to server
	charsWritten = send(socketFD, argv[2], strlen(argv[2]), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(argv[2])) printf("CLIENT: WARNING: Not all data written to socket!\n");

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	// printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	FILE *fp = fopen(argv[1], "r+");
	fseek(fp, 0L, SEEK_END);
	long int sizeofFile = ftell(fp);

	// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	// printf("SIZE OF FILE: %ld\nBUFFER: %ld\n", sizeofFile, strlen(buffer));
	sizeofFile -= strlen(buffer);
	printf("%s", buffer);

	while (sizeofFile != 0) {
		if (strlen(buffer) == 0)
			break;
		memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
		charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
		if (charsRead < 0) error("CLIENT: ERROR reading from socket\n");
		sizeofFile -= strlen(buffer);
		printf("%s", buffer);
	}

	close(socketFD); // Close the socket
}