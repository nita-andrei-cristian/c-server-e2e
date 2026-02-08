#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

int main(){
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (network_socket < 0){
		printf("Creating the socket failed (%d)\n\n", network_socket);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9003);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	if (connection_status < 0){
		printf ("There was an error making a connection to the server (%d) \n\n", connection_status); 
		exit(EXIT_FAILURE);
	}


	char buffer[256] = {0};
	while (1){
		bzero(buffer, sizeof(buffer));

		printf("Enter a message:");
		fgets(buffer, 256, stdin);
		send(network_socket, &buffer, sizeof(buffer), 0);

		bzero(buffer, sizeof(buffer));
		recv(network_socket, &buffer, sizeof(buffer), 0);

		printf("Server responded with [%s]\n", buffer);

		if (strcmp(buffer, "OK") != 0) break;
	}
	
	close(network_socket);
	
	return 0;
	
}
