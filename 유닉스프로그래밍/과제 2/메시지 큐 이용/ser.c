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

// void printMok(char mok[20][38][4]);
int check(char mok[20][38][4]);

// void printMok(char mok[20][38][4]){
// 	for(int i=0;i<20;i++){
// 		for(int j=0;j<38;j++)
// 			printf("%s", mok[i][j]);
// 		printf("\n");
// 	}
// }
int check(char mok[20][38][4]){	// 1 : o승 2 : x승 3: 무승부
	int h_o_count = 0;	// 가로
	int h_x_count = 0;
	int v_o_count = 0;	// 세로
	int v_x_count = 0;
	int d_o_count = 0;	// 오른쪽 아래 대각선
	int d_x_count = 0;
	int d2_o_count = 0;	// 왼쪽 아래 대각선
	int d2_x_count = 0;
	int i = 0, j = 0;
	// bool h_o_check = false;
	// bool h_x_check = false;
	// bool v_o_check = false;
	// bool v_x_check = false;
	// bool d_o_check = false;
	// bool d_x_check = false;
	// bool d2_o_check = false;
	// bool d2_x_check = false;
	int o_checkpoint_v = 50;
	int o_checkpoint_h = 50;
	int x_checkpoint_v = 50;
	int x_checkpoint_h = 50;
	int ii,jj;
	for(i=1;i<20;i++){
		for(j=1;j<20;j++){
			if(strcmp(mok[i][j*2-1] , "O") == 0){
				o_checkpoint_v = i;
				o_checkpoint_h = j*2-1;
				if(i<15 && j<15){
					v_o_count = 0;
					d_o_count = 0;
					d2_o_count = 0;
					for(ii=1;ii<5;ii++){
						h_o_count = 0;
						if(strcmp(mok[o_checkpoint_v+ii][o_checkpoint_h] , "O") == 0)
								v_o_count++;
						for(jj=1;jj<5;jj++){
							if(strcmp(mok[o_checkpoint_v][o_checkpoint_h+jj*2] , "O") == 0)
								h_o_count++;
							if(ii*2 == jj && strcmp(mok[o_checkpoint_v+ii][o_checkpoint_h+jj*2] , "O") == 0)
								d_o_count++;
							if(ii*2 == jj && strcmp(mok[o_checkpoint_v+ii][o_checkpoint_h-jj*2] , "O") == 0)
								d2_o_count++;
						}
					}
					if(h_o_count == 4 || v_o_count == 4 || d_o_count == 4 || d2_o_count == 4)
						return 1;
				}
			}
			if(strcmp(mok[i][j*2-1] , "X") == 0){
				x_checkpoint_v = i;
				x_checkpoint_h = j*2-1;
				if(i<15 && j<15){
					v_x_count = 0;
					d_x_count = 0;
					d2_x_count = 0;
					for(ii=1;ii<5;ii++){
						h_x_count = 0;
						if(strcmp(mok[x_checkpoint_v+ii][x_checkpoint_h] , "X") == 0)
								v_x_count++;
						for(jj=1;jj<5;jj++){
							if(strcmp(mok[x_checkpoint_v][x_checkpoint_h+jj*2] , "X") == 0)
								h_x_count++;
							if(ii*2 == jj && strcmp(mok[x_checkpoint_v+ii][x_checkpoint_h+jj*2] , "X") == 0)
								d_x_count++;
							if(ii*2 == jj && strcmp(mok[x_checkpoint_v+ii][x_checkpoint_h-jj*2] , "X") == 0)
								d2_x_count++;
						}
					}
					if(h_x_count == 4 || v_x_count == 4 || d_x_count == 4 || d2_x_count == 4)
						return 2;
				}
			}
		}
	}
	return 0;
}
void drawmok(char mok[20][38][4]){
	int num = 0;
	char num2 = 'A';
	char buf[256];
	for(int i=0;i<20;i++){
		sprintf(buf,"%2d",num);
		strcpy(mok[i][0] , buf);		
		num++;
		// strcpy(mok[i][0] , "");
	}
	num = 0;
	for(int i=0;i<20;i++){
		if(i < 10 ){
			sprintf(buf,"%d ",num);
			strcpy(mok[0][i] , buf);
			num++;
		}
		else{
			sprintf(buf,"%c ",num2);
			strcpy(mok[0][i] , buf);
			num2++;
		}
		// strcpy(mok[0][i] , "");
	}
	strcpy(mok[1][1] ,"┌");
	strcpy(mok[1][37] ,"┐");
	strcpy(mok[19][1] , "└");
	strcpy(mok[19][37] , "┘");
	for(int i=1;i<20;i++){
		for(int j=1;j<38;j++){
			if(j%2 == 1){
				if(i == 1){
					if(j == 1 || j == 37)
						continue;
					else
						strcpy(mok[i][j] , "┬");
				}
				else if(i == 19){
					if(j == 1 || j == 37)
						continue;
					else
						strcpy(mok[i][j] , "┴");
				}
				else if(j == 1){
					if(i == 1 || i == 19)
						continue;
					else
						strcpy(mok[i][j] , "├");
				}
				else if(j == 37){
					if(i == 1 || i == 19)
						continue;
					else
						strcpy(mok[i][j] , "┤");
				}
				else{
					strcpy(mok[i][j] , "┼");
				}
			}
			else{
				strcpy(mok[i][j], "─");
			}
		}
	}
}
int main(void){
	int x,y;
	int count = 0;
	int gameset;
	int num = 0;
	key_t key;
	int msgid;
	struct mymsgbuf mesg;
	key = ftok("keyfile", 1);
	msgid = msgget(key, IPC_CREAT|0644);
	mesg.mtype = 1;
	char buf[256];
	struct sockaddr_in sin, cli;
	int sd, ns, clientlen = sizeof(cli);
	char mok[20][38][4] = {"",};
	drawmok(mok);
	bool turn = true;
	// if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	// 	perror("socket");
	// 	exit(1);
	// }
	// memset((char *)&sin, '\0', sizeof(sin));
	// sin.sin_family = AF_INET;
	// sin.sin_port = htons(PORTNUM);
	// sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	// if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
	// 	perror("bind");
	// 	exit(1);
	// }

	// if(listen(sd, 5)){
	// 	perror("listen");
	// 	exit(1);
	// }
	// if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen))==-1){
	// 	perror("accept");
	// 	exit(1);
	// }
	do{
		srand(time(NULL));
		x = rand()%10+1;
		sleep(1);
		y = rand()%10+1;
		// scanf("%d %d", &x, &y);
		if(x > 19 || y > 19 || x < 1 || y < 1){
			printf("1~20의 수만 입력해주세요.\n");
			continue;
		}
		if(strcmp(mok[x][y*2-1], "O") == 0 || strcmp(mok[x][y*2-1] , "X") == 0 ){
			printf("돌이 이미 있습니다.\n");
			continue;
		}
		if(turn){
			strcpy(mok[x][y*2-1] , "O");
			turn = !turn;
		}
		else{
			strcpy(mok[x][y*2-1] , "X");
			turn = !turn;
		}
		system("clear");
		// printMok(mok);
		for(int i=0;i<20;i++){
			for(int j=0;j<38;j++){
				// sprintf(buf,mok[i][j],inet_ntoa(cli.sin_addr));
				// sleep(0.2);
				// send(ns,buf,strlen(buf) +1 ,0);
				strcpy(mesg.mtext, mok[i][j]);
				msgsnd(msgid, (void *)&mesg, 80, 0);
				printf("%s", mok[i][j]);
			}
			// sprintf(buf,"\n",inet_ntoa(cli.sin_addr));
			// sleep(0.2);
			// send(ns,buf,strlen(buf) +1 ,0);
			strcpy(mesg.mtext, "\n");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			printf("\n");
		}
		strcpy(mesg.mtext, "EM");
		msgsnd(msgid, (void *)&mesg, 80, 0);
		gameset = check(mok);
		if(gameset == 1){
			// sprintf(buf,"O승리!",inet_ntoa(cli.sin_addr));
			// send(ns,buf,strlen(buf) +1 ,0);
			strcpy(mesg.mtext, "O 승리!\n");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			printf("O 승리!\n");
			break;
		}
		else if(gameset == 2){
			// sprintf(buf,"X승리!",inet_ntoa(cli.sin_addr));
			// send(ns,buf,strlen(buf) +1 ,0);
			strcpy(mesg.mtext, "X 승리!\n");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			printf("X 승리!\n");
			break;
		}
		count ++;
	}while(count<(361/2));
	// close(ns);
	// close(sd);
	return 0;
}

