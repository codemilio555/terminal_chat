all: chat

chat: chat.c send_mess.c send_mess.h
	gcc -g chat.c send_mess.c -o chat

clean: chat
	rm chat

