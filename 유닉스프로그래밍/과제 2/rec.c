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
#include <time.h>
#define PORTNUM 9000

struct mymsgbuf{	// 메세지 버퍼 구조체 선언
	long mtype;
	char mtext[80];
};

struct mymsgbuf inmsg;	// 메세지 버퍼 전역 구조체 선언, 수신용
struct mymsgbuf mesg;	// 메세지 버퍼 전역 구조체 선언, 송신용
struct mymsgbuf sigmsg;	// 메세지 버퍼 전역 구조체 선언, 시그널 송신용
int msgid;				// 수신메시지 id 선언
int mesgid;				// 송신메시지 id 선언
int sigid;				// 시그널메시지 id 선언
int gameset = 0;		// 게임 결과 판별 bool 선언
FILE *fp;				// 파일구조체 fp 선언
struct tm *tm;			// 시간 구조체 tm 선언
time_t t;				// 시간 구조체 t 선언
int starttime;			// 게임 시작 시간 변수 선언
void printhelp(){		// h옵션 사용시 도움말을 출력하는 함수
	printf("\t\t\t도움말\n");
	printf("게임은 Server의 선공으로 시작됩니다.\n");
	printf("가로, 세로, 대각선으로 5개의 돌을 먼저 놓는 사람이 승리합니다.\n");
	printf("별도의 금수 규칙은 없습니다.\n");
	printf("\t\t\t*****주의사항*****\n");
	printf("좌표 입력의 제한시간은 10초이며, 제한시간 초과시 몰수패 처리됩니다.\n");
	printf("게임을 비정상적으로 종료시 몰수패 처리됩니다.\n");
	printf("\t\t\t옵션\n");
	printf("-h : 도움말을 출력합니다.\n");
	printf("-l : 사용자의 log 파일을 출력합니다.\n");
	printf("-b : 사용자의 전적을 출력합니다.\n");
	return;
}
void printlog(){		// l옵션 사용시 로그를 출력할 함수
	fp = fopen(".client_log.log", "r");	// 해당 파일을 읽기 모드로 open
	int fn;								// 내용을 저장할 fn 선언
	while((fn = fgetc(fp)) != EOF){		// 파일의 끝까지 한 character 씩 읽음
		putchar(fn);					// 읽은 내용을 그대로 출력
	}
	fclose(fp);							// 파일 종료
}
void printblog(){		// b옵션 사용시 전적을 출력할 함수
	fp = fopen(".client_log.log", "r");	// 해당 파일을 읽기 모드로 open
	char battle[BUFSIZ];				// BUFSIZ 크기의 char배열 battle 선언
	int w_count = 0;	// 승리 횟수 카운트 변수 선언
	int l_count = 0;	// 패배 횟수 카운트 변수 선언
	int fn;				// 파일의 내용을 저장할 fn
	while((fn = fscanf(fp, "%s", battle)) != EOF){	// 파일의 끝까지 한 문자열씩 읽어 battle에 저장
		if(strcmp(battle, "승리") == 0){ // "승리"를 읽었으면
			w_count++;	// 승리카운트 1 증가
		}
		else if(strcmp(battle, "패배") == 0){	// "패배"를 읽었으면
			l_count++;	// 패배카운트 1 증가
		}
	}
	printf("승리 : %d, 패배 : %d\n", w_count, l_count);	// 승리, 패배 횟수 출력
	fclose(fp);	// 파일 종료
}
void handler(int signo) {	// 지정된 시그널을 처리할 핸들러 함수
	char ans[BUFSIZ];		// 대답을 저장할 char배열 선언
	printf("\n지금 게임을 종료하면 몰수패로 처리됩니다. 게임을 종료하시겠습니까?\n");
	scanf("%s", ans);		// 대답을 입력받음
	if(ans[0] == 'Y' || ans[0] == 'y'){	// 입력받은 대답이 Y,y로 시작하면
		time(&t);			// 현재 시간을 t에 저장
		int endtime = (int)t - starttime;	// 현재 시간에서 starttime을 뺌
		fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 분과 초로 나누어 파일에 기록
		fprintf(fp,"패배\t(강제 종료)\n\n");	// 파일에 강제 종료로 인한 패배를 기록
		fclose(fp);		// 파일 종료
		strcpy(sigmsg.mtext, "QUIT");	// 서버로 QUIT 메시지 전송
		msgsnd(sigid, (void *)&sigmsg, 80, 0);
		exit(1);	// 프로그램 종료
	}
}
void alarmhandler(int signo){	// 알람 시그널 핸들러 함수
	printf("장시간 좌표를 입력하지 않아 몰수패 처리 됩니다.\n");
	time(&t);	// 위와 동일, 시간 처리부분
	int endtime = (int)t - starttime;
	fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);
	fprintf(fp,"패배\t(시간 초과)\n\n");
	fclose(fp);
	strcpy(sigmsg.mtext, "QUIT");	// 위와 동일
	msgsnd(sigid, (void *)&sigmsg, 80, 0);
	exit(1);
}
void readMok(){	// 오목판 메시지를 받는 함수
	int len;
	while(1){	// 무한 루프로 실행
		len = msgrcv(msgid, &inmsg, 80, 0, 0);	// 상대로부터 메시지 받는다.
		if(strcmp(inmsg.mtext, "EM") == 0){	// EM 메시지를 받으면 종료
			return;
		}
		printf("%s", inmsg.mtext);	// 받은 메시지를 출력한다.
	}
}
void gamecheck(){	// 게임의 종료를 판별하는 함수
	int len;
	len = msgrcv(msgid, &inmsg, 80, 0, 0);	// 상대로부터 메시지를 받는다.
	if(strcmp(inmsg.mtext, "XWIN") == 0){	// 받은 메시지가 XWIN 이면
		gameset = 1;	// gameset = 1
		return;			// 함수 종료
	}
	else if(strcmp(inmsg.mtext, "OWIN") == 0){	// 위와 동일, XWIN -> OWIN
		gameset = 2;
		return;
	}
	else{
		if(strcmp(inmsg.mtext, "Notyet") == 0){	// Notyet을 받으면 
			gameset = 0;	// gameset = 0
			return;			// 함수 종료
		}
	}

}
int main(int argc, char *argv[]) {
	char buf[256];	// 전송할 메시지의 문자열을 저장할 buf 선언
	bool turn = false;	// 턴을 판단할 bool 선언
	key_t key;	// 받을 메시지의 key
    key_t key2;	// 보낼 메시지의 key
	key_t key3;	// 시그널 메시지의 key
	int len;	// 메시지를 받을 len
	int x,y;	// clinet의 입력 좌표를 저장할 x, y
	int scannum;	// 입력 좌표의 수를 판별할 scannum
	int n;		// 옵션을 처리할 n 선언
	extern char *optarg;	// 옵션 사용을 위한 변수 선언
	extern int optind;		// 옵션 사용을 위한 변수 선언
	key = 2015;	// key 설정
    key2 = 11053;	// key2 설정
	key3 = 01010011;	// key3 설정
    mesgid = msgget(key2, 0);	// key2로 메시지 큐 id 생성
	msgid = msgget(key, 0);	// key로 메시지 큐 id 생성
	sigid = msgget(key3, 0);	// key3로 메시지 큐 id 생성
	mesg.mtype = 2;		// 보낼 메시지의 type = 2
	sigmsg.mtype = 3;	// 시그널 메시지의 type = 3
	struct sigaction act;	// 강제종료 시그널을 처리할 구조체 act
	struct sigaction act2;	// 알람 시그널을 처리할 구조체 act2
	sigemptyset(&act.sa_mask);	// act의 시그널 집합을 초기화
	sigemptyset(&act2.sa_mask);	// act2의 시그널 집합을 초기화
	sigaddset(&act2.sa_mask, SIGALRM);	// act2 시그널 집합에 SIGALRM 추가
	act.sa_flags = 0;	// flag를 0으로 설정
	act.sa_handler = handler;	// 시그널을 받으면 handler 함수 실행
	act2.sa_flags = 0;	// flag를 0으로 설정
	act2.sa_handler = alarmhandler;	// 시그널을 받으면 alarmhandler 함수 실행
	sigaction(SIGINT, &act, (struct sigaction *)NULL);	// act에 SIGINT 시그널을 적용
	sigaction(SIGQUIT, &act, (struct sigaction *)NULL);	// act에 SIGQUIT 시그널을 적용
	sigaction(SIGTSTP, &act, (struct sigaction *)NULL);	// act에 SIGTSTP 시그널을 적용
	sigaction(SIGALRM, &act2, (struct sigaction *)NULL);	// act2에 SIGALRM 시그널을 적용
	gameset = 0;	// 게임 상태를 0으로 설정
	if(argc > 1){	// 옵션이 존재하면
		while((n = getopt(argc, argv, "hlb")) != -1){
			switch (n){
				case 'h':	// h 옵션 사용시
					printhelp();	// 도움말 출력 함수 실행
					break;
				case 'l':	// l 옵션 사용시
					printlog();	// 로그 출력 함수 실행
					break;
				case 'b':	//  b 옵션 사용시
					printblog();	// 전적 출력 함수 실행
					break;
			}
		}
		return 0;	// main 함수 종료
	}	// 옵션이 없으면 게임 실행
	fp = fopen(".client_log.log", "a");	// 해당 파일을 append 모드로 open
	len = msgrcv(msgid, &inmsg, 80, 0, 0);	// 메시지큐로부터 메시지를 받아옴
	if(strcmp(inmsg.mtext, "Start") == 0){	// 받은 메시지가 "Start"이면
		printf("상대방과 연결되었습니다!\n곧 게임이 시작됩니다.\n");
		strcpy(mesg.mtext, "Connect");	// "Connect" 메시지를 전송
		msgsnd(mesgid, (void *)&mesg, 80, 0);
	}
		time(&t);	// 시간 구조체 t에 현재 시간을 입력
	starttime = (int)t;	// starttime에 현재 시간(초)를 저장
	tm = localtime(&t);	// 현재 시간을 localtime으로 받아 tm 구조체에 저장
	fprintf(fp,"시작 시간 : %04d/%02d/%02d %02d:%02d:%02d\n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	// local 타임을 이용하여 년/월/일 시:분:초 형식으로 게임 시작 시간을 로그파일에 기록
	while(1){	// 무한루프로 실행
		if(gameset == 1){	// XWIN을 받았으면
			printf("승리!\n");	// client의 승리
			time(&t);		// 시간 구조체 t에 현재 시간을 저장
			int endtime = (int)t - starttime;	// 현재시간에서 시작 시간을 빼고 endtime에 저장
			fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 진행된 시간을 분,초 형식으로 로그파일에 기록
			fprintf(fp,"승리\n\n");	// 승리를 로그파일에 기록
			return 0;	// main 종료
		}
		else if(gameset == 2){	// OWIN을 받았으면
			printf("패배!\n");	// client의 패배, 위와 동일
			time(&t);
			int endtime = (int)t - starttime;
			fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);
			fprintf(fp,"패배\n\n");
			return 0;
		}
		if(turn){	// client의 턴이면
			alarm(10);	// 알람을 10초로 설정, 10초 후 alarmhandler 실행
			do{
				printf("좌표를 입력하세요 : ");	// 좌표를 입력받음
				scannum = scanf("%d %d", &y, &x);	// 좌표계를 x,y 좌표계로 사용하기 위해 반대로 입력을 받음
				while (getchar() != '\n');	// 버퍼 삭제
			}while(scannum != 2);	// 두 정수가 입력될 때까지 반복
			if(x > 19 || y > 19 || x < 1 || y < 1){	// 좌표가 범위를 벗어나면
				printf("1~19의 수만 입력해주세요.\n");	
				continue;	// 반복문 재실행, turn은 변하지 않음
			}
			sprintf(buf,"%d",x);	// x좌표를 buf에 저장
			strcpy(mesg.mtext, buf);	// 메시지의 내용을 buf로 설정
			msgsnd(mesgid, (void *)&mesg, 80, 0);	// 메시지 전송
			sprintf(buf,"%d",y);	// 위와 동일
			strcpy(mesg.mtext, buf);
			msgsnd(mesgid, (void *)&mesg, 80, 0);
			strcpy(sigmsg.mtext, "Go");	// Go 메시지를 시그널 메시지 큐로 전송
			msgsnd(sigid, (void *)&sigmsg, 80, 0);
			len = msgrcv(msgid, &inmsg, 80, 0, 0);	// 서버로부터 메시지를 받음
			if(strcmp(inmsg.mtext, "exist") == 0){	// 받은 메시지가 exist이면
				printf("돌이 이미 있습니다.\n");
				continue;	// 이미 해당 좌표에 돌이 있으므로 반복문 재실행
			}
			if(strcmp(inmsg.mtext, "complete") == 0){	// 받은 메시지가 complete이면
				turn = !turn;	// 턴을 변경
				system("clear");	// 화면을 지움
				readMok();	// 오목판을 읽어오는 함수 실행
				gamecheck();	// 게임의 상태를 판단하는 함수 실행
			}
			alarm(3600);	// 알람을 3600초로 설정, alarm은 시간을 재설정 시 기존의 alarm을 대체하므로
							// 10초 내에 입력을 하였다면 상대방의 입력을 기다린다.
							// 다시 자신의 턴이 돌아오면 제한시간은 10초로 재설정된다.
		}
		else{	// 자신의 턴이 아니면
			len = msgrcv(msgid, &inmsg, 80, 0, 0);	// 서버로부터 메시지를 수신
			if(strcmp(inmsg.mtext, "complete") == 0){	// 메시지가 Complete이면
				system("clear");		// 화면을 지움
				readMok();	// 오목판을 읽어오는 함수 실행
				gamecheck();	// 게임의 상태를 판단하는 함수 실행
				turn = !turn;	// 턴을 변경
			}
		}
	}
	fclose(fp);	// 파일 종료
	return 0;	// main 종료
}
