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
#include <signal.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#define PORTNUM 9000

struct mymsgbuf{
	long mtype;
	char mtext[80];
};

struct mymsgbuf inmsg;
struct mymsgbuf mesg;
struct mymsgbuf sigmsg;
int msgid;
int mesgid;
int sigid;
bool isend = false;
int gameset = 0;
FILE *fp;

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
void printhelp(){
	printf(" 도움말 예정입니다. \n");
	return;
}
void printlog(){
	fp = fopen(".client_log.log", "r");
	int fn;
	while((fn = fgetc(fp)) != EOF){
		putchar(fn);
	}
	fclose(fp);
}
void printblog(){
	fp = fopen(".client_log.log", "r");
	char battle[BUFSIZ];
	int w_count = 0;
	int l_count = 0;
	int fn;
	while((fn = fscanf(fp, "%s", battle)) != EOF){
		if(strcmp(battle, "승리") == 0){
			w_count++;
		}
		else if(strcmp(battle, "패배") == 0){
			l_count++;
		}
	}
	printf("승리 : %d, 패배 : %d\n", w_count, l_count);
	fclose(fp);
}
void handler(int signo) {
	char ans[BUFSIZ];
	printf("\n지금 게임을 종료하면 몰수패로 처리됩니다. 게임을 종료하시겠습니까?\n");
	scanf("%s", ans);
	if(ans[0] == 'Y' || ans[0] == 'y'){
		fprintf(fp,"패배\n");
		fclose(fp);
		strcpy(sigmsg.mtext, "QUIT");
		msgsnd(sigid, (void *)&sigmsg, 80, 0);
		exit(1);
	}
}
void alarmhandler(int signo){
	printf("장시간 좌표를 입력하지 않아 몰수패 처리 됩니다.\n");
	fprintf(fp,"패배\n");
	fclose(fp);
	strcpy(sigmsg.mtext, "QUIT");
	msgsnd(sigid, (void *)&sigmsg, 80, 0);
	exit(1);
}
void readMok(){
	int len;
	while(1){
		len = msgrcv(msgid, &inmsg, 80, 0, 0);
		if(strcmp(inmsg.mtext, "EM") == 0){
			return;
		}
		printf("%s", inmsg.mtext);
	}
}
void gamecheck(){
	int len;
	len = msgrcv(msgid, &inmsg, 80, 0, 0);
	if(strcmp(inmsg.mtext, "XWIN") == 0){
		// printf("X 승리!\n");
		// isend = true;
		gameset = 1;
		return;
	}
	else if(strcmp(inmsg.mtext, "OWIN") == 0){
		// printf("O 승리!\n");
		// isend = true;
		gameset = 2;
		return;
	}
	else{
		if(strcmp(inmsg.mtext, "Notyet") == 0){
			gameset = 0;
			return;
		}
	}

}
int main(int argc, char *argv[]) {
	int sd;
	char buf[256];
	struct sockaddr_in sin;
	bool turn = false;
	key_t key;
    key_t key2;
	key_t key3;
	int len;
	int x,y;
	int semid;
	int scannum;
	int n;
	extern char *optarg;
	extern int optind;
	pid_t pid = getpid();
	semid = initsem(1);
	key = 2015;
    key2 = 11053;
	key3 = 01010011;
    mesgid = msgget(key2, 0);
	msgid = msgget(key, 0);
	sigid = msgget(key3, 0);
	mesg.mtype = 2;
	sigmsg.mtype = 3;
	struct sigaction act;
	struct sigaction act2;
	sigemptyset(&act.sa_mask);
	sigemptyset(&act2.sa_mask);
	sigaddset(&act2.sa_mask, SIGALRM);
	act.sa_flags = 0;
	act.sa_handler = handler;
	act2.sa_flags = 0;
	act2.sa_handler = alarmhandler;
	sigaction(SIGINT, &act, (struct sigaction *)NULL);
	sigaction(SIGQUIT, &act, (struct sigaction *)NULL);
	sigaction(SIGTSTP, &act, (struct sigaction *)NULL);
	sigaction(SIGALRM, &act2, (struct sigaction *)NULL);
	gameset = 0;
	if(argc > 1){
		while((n = getopt(argc, argv, "hlb")) != -1){
			switch (n){
				case 'h':
					printhelp();
					break;
				case 'l':
					printlog();
					break;
				case 'b':
					printblog();
					break;
			}
		}
		return 0;
	}
	fp = fopen(".client_log.log", "a");
	system("date >> .client_log.log");
	len = msgrcv(msgid, &inmsg, 80, 0, 0);
	if(strcmp(inmsg.mtext, "Start") == 0){
		printf("상대방과 연결되었습니다!\n곧 게임이 시작됩니다.\n");
		strcpy(mesg.mtext, "Connect");
		msgsnd(mesgid, (void *)&mesg, 80, 0);
	}
	while(1){
		if(gameset == 1){
			printf("승리!\n");
			fprintf(fp,"승리\n");
			return 0;
		}
		else if(gameset == 2){
			printf("패배!\n");
			fprintf(fp,"패배\n");
			return 0;
		}
		if(turn){
			alarm(10);
			do{
				printf("좌표를 입력하세요 : ");
				scannum = scanf("%d %d", &y, &x);
				while (getchar() != '\n');
			}while(scannum != 2);
			if(x > 19 || y > 19 || x < 1 || y < 1){
				printf("1~20의 수만 입력해주세요.\n");
				continue;
			}
			sprintf(buf,"%d",x);
			strcpy(mesg.mtext, buf);
			msgsnd(mesgid, (void *)&mesg, 80, 0);
			sprintf(buf,"%d",y);
			strcpy(mesg.mtext, buf);
			msgsnd(mesgid, (void *)&mesg, 80, 0);
			strcpy(sigmsg.mtext, "Go");
			msgsnd(sigid, (void *)&sigmsg, 80, 0);
			len = msgrcv(msgid, &inmsg, 80, 0, 0);
			if(strcmp(inmsg.mtext, "exist") == 0){
				printf("돌이 이미 있습니다.\n");
				continue;
			}
			if(strcmp(inmsg.mtext, "complete") == 0){
				turn = !turn;
				system("clear");
				readMok();
				gamecheck();
			}
			alarm(3600);
		}
		else{
			len = msgrcv(msgid, &inmsg, 80, 0, 0);
			if(strcmp(inmsg.mtext, "complete") == 0){
				system("clear");
				readMok();
				gamecheck();
				turn = !turn;
			}
		}
	}
	fclose(fp);
	return 0;
}
