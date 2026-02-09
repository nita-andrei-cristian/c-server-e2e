#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 9009

int main(){
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (network_socket < 0){
		printf("Creating the socket failed (%d)\n\n", network_socket);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	if (connection_status < 0){
		printf ("There was an error making a connection to the server (%d) \n\n", connection_status); 
		exit(EXIT_FAILURE);
	}


	int buffer_size = sizeof(char) * 32;
	// client buffer format
	// [0] = 0-255 character (mod character)
	// [1 -> n-1] = message
	// [n] = \0
	// n < 256
	
	char server_response[256];

	while (1){
		char *buffer = (char*)malloc(buffer_size);

		printf("Select an action:\n[0] : send message\n[1] : quit\n");
		int input;
		scanf("%d *[^\n]", &input);
		fflush(stdin);

		if (input == 0){
			*buffer = '0';
			printf("Enter a message:");
			fgets(buffer+1, buffer_size-1, stdin); // ofser by one to exclude the mod
		}else if(input == 1){
			*buffer = '1';
		}
	
		send(network_socket, buffer, buffer_size, 0);
		
		free(buffer);

		bzero(server_response, sizeof(server_response));
		recv(network_socket, &server_response, sizeof(server_response), 0);

		printf("Server responded with [%s]\n", server_response);

		if (strcmp(server_response, "OK") != 0) break;
	}
	
	close(network_socket);
	
	return 0;
	
}
