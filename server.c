#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include <pthread.h>
#include <semaphore.h>

#define PORT 9009
#define MAX_CLIENTS 5

sem_t mutex;

typedef struct {
	int fd;
} client_job;

void* client_thread(void* args){
	client_job *input = args;

	sem_wait(&mutex);
	printf("Sempahore starts for client (%d) \n", input->fd);

	int buffer_size = sizeof(char) * 32;
	int response_size = sizeof(char) * 8;

	while(1){
		printf("Waiting for client response...\n");
		char *buffer = malloc(buffer_size);
	
		char userInput = *buffer;
		char *payload = (buffer+1);
	
		if (userInput == 1){ break; }

		recv(input->fd, buffer, buffer_size, 0);
		printf("Server : Client [%d] sent : [%d] : %s\n", input->fd, userInput, payload);

		char *response = (char*)malloc(response_size);

		strcpy(response, "OK");
		if (userInput == '1'){
			strcpy(response, "NO");
		};
	
		send(input->fd, response, response_size, 0);
		free(response); 
	}
	
	printf("Semaphore quiting for client (%d)..\n", input->fd);
	sem_post(&mutex);
	close(input->fd);
	
	free(input);

}

int main(){


	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (server_socket < 0){
		printf("Creating the socket failed (%d)\n\n", server_socket);
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0){
		printf("Binding the socket failed!\n\n");
		exit(EXIT_FAILURE);
	}

	if (listen(server_socket, MAX_CLIENTS) < 0){
		printf("Socket listening failed\n\n");
		exit(EXIT_FAILURE);
	}

	sem_init(&mutex, 0, MAX_CLIENTS);
	pthread_t clients_threads[MAX_CLIENTS];
	int clients_fd[MAX_CLIENTS];
	int clients_len = 0;

	while (1){
		if (clients_len >= MAX_CLIENTS){
			printf("Client cap reached");
			break;
		}
		int fd =  accept(server_socket,NULL,NULL);
		clients_fd[clients_len] = fd;
		if (fd < 0){
			printf("Client socket is negative, couldn't accept");
			exit(EXIT_FAILURE);
		}
		printf("[server] : found client (%d) willing to join. Index (%d) \n", fd, clients_len);
		
		// add client to thread
		client_job *args = malloc (sizeof *args);
		args->fd = fd;

		if (pthread_create(&clients_threads[clients_len], NULL, client_thread, args) != 0){
			printf("[server] : failed to create thread for client (%d)\n", fd);
			free(args); // in case if it fails, prevent segmentation fault;
			clients_len--;
		}else{
			printf("[server] : client (%d) successfully joined\n", fd);
		}
		clients_len++;
	}

	for (int i = 0; i < clients_len; i++){
		pthread_join(clients_threads[i], NULL);
	}

	sem_destroy(&mutex);
	
	close(server_socket);

	
	return 0;
	
}
