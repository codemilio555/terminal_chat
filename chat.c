#include <string.h>
#include "send_mess.h"
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>




int disp_high= 10;
char input_mess[128];
char name[30];
char ip[15];
int run =0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void own_mess(char *name, char *mess);

void* input(){
	while(run==0){
		pthread_mutex_lock(&lock);
	
		printf("\033[%d;1HInput\r", disp_high+2);
		fflush(stdout);

		pthread_mutex_unlock(&lock);
		if(fgets(input_mess, 127, stdin)){
			printf("\033[%d;1H\033[K", disp_high+2);
			fflush(stdout);
			own_mess(name, input_mess);

			send_mess(input_mess);
			continue;
		}
	}
	return NULL;
}


void get_mess(char *from, char *mess){

	pthread_mutex_lock(&lock);
	printf("\033[%d;1H", disp_high);
	printf("\033[K");
	
	printf("%s:\n%s\n", from, mess);


	fflush(stdout);
	disp_high+=2;
	pthread_mutex_unlock(&lock);
}

void own_mess(char *ownName, char *mess){
	pthread_mutex_lock(&lock);

	printf("\033[%d;1H", disp_high);
	printf("\033[K");

	printf("\t\t%s:\n\t%s\n", ownName, mess);

	fflush(stdout);
	disp_high+=2;

	pthread_mutex_unlock(&lock);
}

int main(int args, char *argc[]){
	if(args!=3){
		printf("Usage: chat <ip> <username>\n");
		return 1;
	}
	strcpy(ip, argc[1]);
	strcpy(name, argc[2]);
	pthread_t input_t;	

	printf("\033[2J");

	pthread_create(&input_t, NULL, input, NULL);
	
	on_mess(get_mess);
	
	pthread_join(input_t, NULL);

}

