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
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#define PORTNUM 9000

struct mymsgbuf{
	long mtype;
	char mtext[80];
};
struct mymsgbuf mesg;
struct mymsgbuf inmsg;
struct mymsgbuf sigmsg;
int msgid;
int mesgid;
int sigid;
bool quit = false;
union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int initsem(key_t semkey){
	union semun semunarg;
	int status = 0, semid;

	semid = semget(semkey, 1, IPC_CREAT | IPC_EXCL | 0600);
	if(semid == -1){
		if(errno == EEXIST)
			semid = semget(semkey, 1, 0);
	}
	else{
		semunarg.val = 1;
		status = semctl(semid, 0, SETVAL, semunarg);
	}

	if (semid == -1 || status == -1){
		perror("initsem");
		return (-1);
	}
	return semid;
}

int semlock(int semid){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	if(semop(semid, &buf, 1) == -1){
		perror("semlock failed");
		exit(1);
	}
	return 0;
}

int semunlock(int semid){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	if(semop(semid, &buf, 1) == -1){
		perror("semunlock failed");
		exit(1);
	}
	return 0;
}

void semhandle(){
	int semid;
	pid_t pid = getpid();

	if((semid = initsem(1)) < 0)
		exit(1);

	semlock(semid);
	printf("Lock : Process %d\n", (int)pid);
	printf("** Lock Mode : Criticl Section\n");
	sleep(5);
	printf("Unlock : Process %d\n", (int)pid);
	semunlock(semid);

	exit(0);
}
void printMok(char mok[20][38][4]);
int check(char mok[20][38][4]);

void printMok(char mok[20][38][4]){
	int stopsig = msgrcv(sigid, &sigmsg, 80, 0, IPC_NOWAIT);	
	if(strcmp(sigmsg.mtext, "QUIT") == 0){
		quit = true;
		return;
	}
	mesg.mtype = 1;
	for(int i=0;i<20;i++){
		for(int j=0;j<38;j++){
			strcpy(mesg.mtext, mok[i][j]);
			msgsnd(msgid, (void *)&mesg, 80, 0);
			printf("%s", mok[i][j]);
		}
		strcpy(mesg.mtext, "\n");
		msgsnd(msgid, (void *)&mesg, 80, 0);
		printf("\n");
	}
}
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
	int o_checkpoint_v = 50;
	int o_checkpoint_h = 50;
	int x_checkpoint_v = 50;
	int x_checkpoint_h = 50;
	int ii,jj;
	if(quit){
		return 4;
	}
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
							if(ii == jj && strcmp(mok[o_checkpoint_v+ii][o_checkpoint_h+jj*2] , "O") == 0)
								d_o_count++;
							if(ii == jj && strcmp(mok[o_checkpoint_v+ii][o_checkpoint_h-jj*2] , "O") == 0)
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
							if(ii == jj && strcmp(mok[x_checkpoint_v+ii][x_checkpoint_h+jj*2] , "X") == 0)
								d_x_count++;
							if(ii == jj && strcmp(mok[x_checkpoint_v+ii][x_checkpoint_h-jj*2] , "X") == 0)
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
	FILE *fp;
	fp = fopen(".server_log.log", "a");
	int cx,cy;	// client의 좌표
	int count = 0;
	int gameset;
	int num = 0;
	key_t key;
    key_t key2;
	key_t key3;
	int semid;
	pid_t pid = getpid();
	semid = initsem(1);
	key = 2015;
    key2 = 11053;
	key3 = 01010011;
	msgid = msgget(key, IPC_CREAT|0644);
    mesgid = msgget(key2, IPC_CREAT|0644);
	sigid = msgget(key3, IPC_CREAT|0644);
    inmsg.mtype = 2;
	mesg.mtype = 1;
	int len;
	int scannum = 0;
	char buf[256];
	struct sockaddr_in sin, cli;
	int sd, ns, clientlen = sizeof(cli);
	char mok[20][38][4] = {"",};
	drawmok(mok);
	bool turn = true;

	strcpy(mesg.mtext, "Start");
	msgsnd(msgid, (void *)&mesg, 80, 0);
	printf("상대를 기다리고 있습니다...\n");
	len = msgrcv(mesgid, &inmsg, 80, 0, 0);
	if(strcmp(inmsg.mtext, "Connect") == 0){
		printf("상대방과 연결되었습니다.\n곧 게임이 시작됩니다.\n");
	}
	else{
		printf("연결에 오류가 발생하였습니다.\n프로그램을 종료합니다.\n");
		msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(1);
	}
	int stopsig = msgrcv(sigid, &sigmsg, 80, 0, IPC_NOWAIT);
	do{
		// srand(time(NULL));
		// x = rand()%10+1;
		// sleep(1);
		// y = rand()%10+1;
		if(turn){
			do{
				printf("좌표를 입력하세요 : ");
				scannum = scanf("%d %d", &y, &x);
				while (getchar() != '\n');
			}while(scannum != 2);
		}
		else{
			stopsig = msgrcv(sigid, &sigmsg, 80, 0, 0);
			if(strcmp(sigmsg.mtext, "QUIT") == 0){
				printf("상대방이 게임을 종료하였습니다.\n");
				printf("몰수승으로 처리됩니다.\n");
				system("date >> .server_log.log");
				fprintf(fp, "승리\n");
				sleep(1);
				break;
			}
			len = msgrcv(mesgid, &inmsg, 80, 0, 0);
			char *string = inmsg.mtext;
			cx = atoi(string);
			len = msgrcv(mesgid, &inmsg, 80, 0, 0);
			string = inmsg.mtext;
			cy = atoi(string);
		}
		if(x > 19 || y > 19 || x < 1 || y < 1){
			printf("1~20의 수만 입력해주세요.\n");
			continue;
		}
		if(turn && strcmp(mok[x][y*2-1], "O") == 0){
			printf("돌이 이미 있습니다.\n");
			continue;
		}
		if(turn && strcmp(mok[x][y*2-1] , "X") == 0 ){
			printf("돌이 이미 있습니다.\n");
			continue;
		}
		if(!turn && strcmp(mok[cx][cy*2-1], "O") == 0){
			strcpy(mesg.mtext, "exist");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			continue;
		}
		if(!turn && strcmp(mok[cx][cy*2-1] , "X") == 0 ){
			strcpy(mesg.mtext, "exist");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			continue;
		}
		if(turn){
			strcpy(mok[x][y*2-1] , "O");
			strcpy(mesg.mtext, "complete");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			turn = !turn;
		}
		else{
			strcpy(mok[cx][cy*2-1] , "X");
			strcpy(mesg.mtext, "complete");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			turn = !turn;
		}
		system("clear");
		printMok(mok);
		strcpy(mesg.mtext, "EM");
		msgsnd(msgid, (void *)&mesg, 80, 0);
		gameset = check(mok);
		if(gameset == 1){
			strcpy(mesg.mtext, "OWIN");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			system("date >> server_log.log");
			printf("승리!\n");
			fprintf(fp, "승리\n");
			sleep(1);
			break;
		}
		else if(gameset == 2){
			strcpy(mesg.mtext, "XWIN");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			system("date >> server_log.log");
			printf("패배!\n");
			fprintf(fp,"패배\n");
			sleep(1);
			break;
		}
		else if(gameset == 4){
			printf("상대방이 게임을 종료하였습니다.\n");
			printf("몰수승으로 처리됩니다.\n");
			system("date >> .server_log.log");
			fprintf(fp, "승리\n");
			sleep(1);
			break;
		}
		else{
			strcpy(mesg.mtext, "Notyet");
			msgsnd(msgid, (void *)&mesg, 80, 0);
		}
		count ++;
	}while(count<(361/2));
	fclose(fp);
	msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);
	msgctl(mesgid, IPC_RMID, (struct msqid_ds *)NULL);
	msgctl(sigid, IPC_RMID, (struct msqid_ds *)NULL);
	return 0;
}

