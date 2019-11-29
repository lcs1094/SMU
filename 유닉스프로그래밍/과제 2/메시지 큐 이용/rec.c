#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <sys/msg.h>
#define PORTNUM 9000

struct mymsgbuf{
	long mtype;
	char mtext[80];
};

int main(void) {
	int sd;
	char buf[256];
	struct sockaddr_in sin;
	struct mymsgbuf inmsg;
	key_t key;
	int msgid, len;

	key = ftok("keyfile", 1);

	// if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	// 	perror("socket");
	// 	exit(1);
	// }
	// memset((char *)&sin, '\0', sizeof(sin));
	// sin.sin_family = AF_INET;
	// sin.sin_port = htons(PORTNUM);
	// sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	// if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
	// 	perror("connect");
	// 	exit(1);
	// }
	while(1){
		// if (recv(sd, buf, sizeof(buf), 0) == -1) {
		// 	perror("recv");
		// 	exit(1);
		// }
		msgid = msgget(key, 0);
		len = msgrcv(msgid, &inmsg, 80, 0, 0);
		if(strcmp(inmsg.mtext, "EM") == 0){
			system("clear");
		}
		else{
			printf("%s", inmsg.mtext);
		}
	}
	// close(sd);
	// printf("From Server : %s\n", buf);
	return 0;
}
