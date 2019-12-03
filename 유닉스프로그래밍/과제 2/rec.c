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
int msgid;
int mesgid;
bool isend = false;

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

void handler(int signo) {
	char ans[BUFSIZ];
	printf("\n지금 게임을 종료하면 몰수패로 처리됩니다. 게임을 종료하시겠습니까?\n");
	scanf("%s", ans);
	if(ans[0] == 'Y' || ans[0] == 'y'){
		mesg.mtype = 2;
		strcpy(mesg.mtext, "Quit");
		msgsnd(mesgid, (void *)&mesg, 80, IPC_NOWAIT);
		exit(1);
	}
}
void readMok(){
	int len;
	while(1){
		len = msgrcv(msgid, &inmsg, 80, 0, 0);
		if(strcmp(inmsg.mtext, "EM") == 0){
			return;
		}
		if(strcmp(inmsg.mtext, "XWIN") == 0){
			printf("X 승리!\n");
			isend = true;
			return;
		}
		else if(strcmp(inmsg.mtext, "OWIN") == 0){
			printf("O 승리!\n");
			isend = true;
			return;
		}
		printf("%s", inmsg.mtext);
	}
}
int main(void) {
	int sd;
	char buf[256];
	struct sockaddr_in sin;
	bool turn = false;
	key_t key;
    key_t key2;
	int len;
	int x,y;
	int semid;
	pid_t pid = getpid();
	semid = initsem(1);
	key = 2015;
    key2 = 11053;
    mesgid = msgget(key2, 0);
	msgid = msgget(key, 0);
	mesg.mtype = 2;
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGQUIT);
	act.sa_flags = SA_RESETHAND;
	act.sa_handler = handler;
	if (sigaction(SIGINT, &act, (struct sigaction *)NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	len = msgrcv(msgid, &inmsg, 80, 0, 0);
	if(strcmp(inmsg.mtext, "Start") == 0){
		printf("상대방과 연결되었습니다!\n곧 게임이 시작됩니다.\n");
		strcpy(mesg.mtext, "Connect");
		msgsnd(mesgid, (void *)&mesg, 80, 0);
	}
	while(1){
		if(isend){
			return 0;
		}
		if(turn){
			scanf("%d %d", &x, &y);
			sprintf(buf,"%d",x);
			strcpy(mesg.mtext, buf);
			msgsnd(mesgid, (void *)&mesg, 80, 0);
			sprintf(buf,"%d",y);
			strcpy(mesg.mtext, buf);
			msgsnd(mesgid, (void *)&mesg, 80, 0);
			len = msgrcv(msgid, &inmsg, 80, 0, 0);
			if(x > 19 || y > 19 || x < 1 || y < 1){
				printf("1~20의 수만 입력해주세요.\n");
				continue;
			}
			if(strcmp(inmsg.mtext, "exist") == 0){
				printf("돌이 이미 있습니다.\n");
				continue;
			}
			if(strcmp(inmsg.mtext, "complete") == 0){
				turn = !turn;
				system("clear");
				readMok();
			}
		}
		else{
			len = msgrcv(msgid, &inmsg, 80, 0, 0);
			if(strcmp(inmsg.mtext, "complete") == 0){
				system("clear");
				readMok();
				turn = !turn;
			}
		}
	}
	return 0;
}
