#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

#define PORT 9001
#define PACKET_SIZE 1024

int main()
{
	int socketId;
	struct sockaddr_in server;

	char textToSend[PACKET_SIZE] = {0}, textToRec[PACKET_SIZE] = {0};

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	socketId = socket(AF_INET, SOCK_STREAM, 0);
	
	// Client initiates the connection.
	if(connect(socketId, (struct sockaddr * )&server, sizeof(server)) < 0){
		printf("Couldn't connect to Server. Exiting...\n");
		exit(1);
	}
	printf("You are now connected to server\n");

	// Client takes file name from the user.
	char fileName[PACKET_SIZE] = {0};
	printf("Enter file name: ");
	scanf("%s", fileName);

	// Client sends file name to the Server.
	strcpy(textToSend, fileName);
	send(socketId, textToSend, strlen(textToSend), 0);
	printf("Text sent! Now Reading \n");

	// Client gets the file size.
	int fileSize;
	if(read(socketId, textToRec, PACKET_SIZE) > 0){
		sscanf(textToRec, "%d", &fileSize);
		memset(textToRec, 0, PACKET_SIZE);
	} else {
		printf("Server disconnected. Exiting...\n");
		exit(1);
	}
	

	if(fileSize == -1){
		printf("File not found. Exiting...\n");

	} else {
		int actuallyRead;
		FILE *file;

		// Client saves the file on local drive.
		if(file = fopen(fileName, "wb")){
			
			while(fileSize > 0){
				if(fileSize > PACKET_SIZE)
					actuallyRead = read(socketId, textToRec, PACKET_SIZE);
				else
					actuallyRead = read(socketId, textToRec, fileSize);

				fwrite(textToRec, sizeof(char), actuallyRead, file);
				memset(textToRec, 0, PACKET_SIZE);
				fileSize -= actuallyRead;
			}

			printf("File received!\n");
			fclose(file);

		} else {
			printf("Error writing file. Exiting...");
		}
	}
	
	// Client closes the connection.
	close(socketId);
	return 0;
}
