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
	int socketId, client;
	struct sockaddr_in server;
	int addLen = sizeof(server);

	char textToSend[PACKET_SIZE] = {0}, textToRec[PACKET_SIZE] = {0};

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	socketId = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(socketId, (struct sockaddr *)&server, sizeof(server)) < 0){
		printf("Couldn't bind to socket. Exiting...\n");
		exit(1);
	}
	
	listen(socketId, 0);
	printf("Server ready.\n");
	while(1){
		printf("Waiting for Client...\n");

		// Server gets connected to the client.
		client = accept(socketId, (struct sockaddr *)&server, (socklen_t *)&addLen);
		printf("Client Connected \n");

		// Server gets the file name.
		read(client, textToRec, sizeof(textToRec));
		printf("File requested : %s\n", textToRec);

		FILE *file;
		int fileSize = -1;

		// Server searches for the file,
		if(file = fopen(textToRec, "rb")){
			printf("File found. Sending...\n");

			// Server calculates the file size.
			fseek(file, 0, SEEK_END);
			fileSize = ftell(file);
			fseek(file, 0, SEEK_SET);

			// Server sends the file size.
			sprintf(textToSend, "%d", fileSize);
			send(client, textToSend, PACKET_SIZE, 0);
			memset(textToSend, 0, PACKET_SIZE);

			int actuallyRead;

			// Server sends file to the client.
			while(fileSize > 0){
				if(fileSize > PACKET_SIZE)
					actuallyRead = fread(textToSend, sizeof(char), PACKET_SIZE, file);
				else
					actuallyRead = fread(textToSend, sizeof(char), fileSize, file);
				
				send(client, textToSend, actuallyRead, 0);
				memset(textToSend, 0, PACKET_SIZE);
				fileSize -= actuallyRead;
			}

			printf("File Sent!\n");
			fclose(file);
			
		} else {
			printf("File not found. Disconnecting\n");
			
			sprintf(textToSend, "%d", fileSize);
			send(client, textToSend, strlen(textToSend), 0);
		}
		
		// Since socket is in blocking mode by default, read() makes
		// the Server wait for Client to close the connection.
		read(client, textToRec, PACKET_SIZE);
		close(client);

		printf("Client disconnected. Connect again? (Y/N): ");
		char response;
		scanf("%s", &response);
		if(response == 'N')
			break;
	}

	// Server closes the socket
	close(socketId);
	return 0;
}
