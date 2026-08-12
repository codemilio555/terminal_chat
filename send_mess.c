#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "send_mess.h"

#ifndef name
char name[30] = "Error";
#endif
#ifndef ip
char ip[]="127.0.0.1";
#endif

int sock=0;

void read_newline(char *buff,int buffsize){
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
		}
	}
	buff[written]=0;
}

void on_mess(void (*onf) (char*, char*)){
	sock= socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serv_addr;

	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port = htons(8000);	
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))>0){
		perror("connect to server");
		exit(1);
	}

	send(sock, name, strlen(name)+1, 0);
	char buff[1024];	
	while(1){
		read_newline(buff, 1023);

		char *from = strtok(buff, " ");
		char *mess = strtok(NULL, "\n");
		onf(from, mess);
		
	}
}

void send_mess(char *message){
	send(sock, message, strlen(message)+1,0);

	
}
