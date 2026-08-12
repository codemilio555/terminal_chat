#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define max_clients 5
int clients[max_clients];

pthread_mutex_t client_arr_lock = PTHREAD_MUTEX_INITIALIZER;


int read_newline(int sock, char *buff,int buffsize){
	int written = 0;
	char c;

	while(written < buffsize){
		int n = recv(sock, &c, 1, 0);
		if(n > 0){
			if(c==0){
				break;
			}
			buff[written]=c;
			written++;
		}else{
			return 0;
		}
	}
	buff[written]=0;
	return 1;
}


void send_all(int id, char *from, char *mess){
	pthread_mutex_lock(&client_arr_lock);
	char *to_send = malloc(strlen(from) +strlen(mess)+2);
	sprintf(to_send, "%s %s", from,mess);
	for(int i =0;i<max_clients; i++){
		if(i==id)continue;
		if(clients[i]==0)continue;

		

		
		
		send(clients[i], to_send, strlen(to_send)+1, 0);

	}
	free(to_send);
	pthread_mutex_unlock(&client_arr_lock);
}

int find_client(){
	pthread_mutex_lock(&client_arr_lock);
	int id=0;
	for(int i=0;i<max_clients;i++){
		if(clients[i]==0){
			clients[i]=1;
			id=i;
			break;
		}
	}
	pthread_mutex_unlock(&client_arr_lock);
	return id;
}
void* handleClient(void* client){
	printf("New client!\n");
	
	int id=find_client();
	int client_fd = *(int*)client;
	free(client);
	pthread_mutex_lock(&client_arr_lock);

	clients[id]=client_fd;
	pthread_mutex_unlock(&client_arr_lock);

	char buffer[512];
	char name[32];
	int n=0;
	
	//get name
	read_newline(client_fd, name, 31);
	
	while(read_newline(client_fd,buffer,511)>0){
		send_all(id, name, buffer);
	}	
	printf("Client disconnected\n");
	clients[id]=0;
	return NULL;
}

int main(){
	int server_fd, client_fd;
	struct sockaddr_in address;

	char buffer[512];

	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	address.sin_port=htons(8000);

	bind(server_fd, (struct sockaddr*)&address, sizeof(address));
	//die verbindung setzen

	listen(server_fd, max_clients);


	while(1){
		int new_fd = accept(server_fd, NULL,NULL);

		pthread_t thread;
		
		int *fd = malloc(sizeof(int));
		*fd=new_fd;
		pthread_create(&thread, NULL, handleClient, fd);

		pthread_detach(thread);
	}
}
