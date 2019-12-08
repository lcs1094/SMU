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

struct mymsgbuf{	// 메시지 버퍼 정의
	long mtype;		// 메시지 타입
	char mtext[80];	// 메시지 내용, 최대 80자
};
struct mymsgbuf mesg;	// 전역변수 구조체 mesg 선언, 송신용
struct mymsgbuf inmsg;	// 전역변수 구조체 inmsg 선언, 수신용
struct mymsgbuf sigmsg;	// 전역변수 구조체 sigmsg 선언, 시그널 수신용
int msgid;				// 전역변수 msgid 선언, 송신 mesg의 id
int mesgid;				// 전역변수 mesgid 선언, 수신 inmsg의 id
int sigid;				// 전역변수 sigid 선언, 시그널 수신 sigid의 id
bool quit = false;		// 종료 시그널을 받으면 true로 설정할 bool 선언
struct tm *tm;			// 시간 구조체 tm 선언
time_t t;				// 시간 구조체 t 선언
int starttime;			// 게임 시작 시간 변수 선언
void printMok(char mok[20][38][4]);	// 오목판을 출력하는 함수 printMok 선언
int check(char mok[20][38][4]);		// 오목의 경기 종료를 판단할 함수 check 선언
FILE *fp;	// 로그파일을 설정할 파일 구조체 fp 선언

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
		strcpy(mesg.mtext, "QUIT");	// 클라이언트로 QUIT 메시지 전송
		msgsnd(msgid, (void *)&mesg, 80, 0);
		printf("종료를 기다리는 중입니다...\n");
		sleep(1);	// 클라이언트의 메시지 처리 대기
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
	strcpy(mesg.mtext, "QUIT");	// 위와 동일
	msgsnd(msgid, (void *)&mesg, 80, 0);
	printf("종료를 기다리는 중입니다...\n");
	sleep(1);	// 클라이언트의 메시지 처리 대기
	exit(1);
}

void printMok(char mok[20][38][4]){	// printMok 정의
// 특수문자는 2바이트이므로 3차원 배열을 통하여 오목판을 정의하였음
	int stopsig = msgrcv(sigid, &sigmsg, 80, 0, IPC_NOWAIT);
	// 메시지큐 sigmsg에 메시지가 있으면 stopsig에 저장, 아니면 계속 진행
	if(strcmp(sigmsg.mtext, "QUIT") == 0){	// 메시지가 존재하고 그 메시지가 QUIT이면
		quit = true;	// quit을 true로 설정
		return;			// 함수 종료
	}
	mesg.mtype = 1;		// 보낼 메시지의 mtype을 1로 설정
	for(int i=0;i<20;i++){		// 오목판의 칸수 만큼 반복
		for(int j=0;j<38;j++){
			strcpy(mesg.mtext, mok[i][j]);	// 각 원소의 내용을 메시지 내용으로 복사
			msgsnd(msgid, (void *)&mesg, 80, 0);	// 메시지 버퍼 mesg를 msgid를 가지는 메시지 큐로 전송
			printf("%s", mok[i][j]);	// 각 원소의 내용을 화면에 출력
		}
		strcpy(mesg.mtext, "\n");		// 한 행을 출력 후 개행문자를 복사
		msgsnd(msgid, (void *)&mesg, 80, 0);	// 복사한 메시지를 전송
		printf("\n");					// 개행 출력
	}
}
int check(char mok[20][38][4]){	// 1 : o승 2 : x승 4: 몰수승
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
	if(quit){		// quit 이 true이면
		return 4;	// 4를 반환하고 함수 종료(몰수승일 경우)
	}
	for(i=1;i<=20;i++){
		for(j=1;j<=20;j++){
			if(strcmp(mok[i][j*2-1] , "O") == 0){ // O를 발견하면 checkpoint로 저장
				o_checkpoint_v = i;
				o_checkpoint_h = j*2-1;
				v_o_count = 0;	// 세로 연속 카운트
				d_o_count = 0;	// 우하향 대각선 연속 카운트
				d2_o_count = 0;	// 좌하향 대각선 연속 카운트
				for(ii=1;ii<5;ii++){
					h_o_count = 0;	// 가로 연속 카운트
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
					return 1;	// 카운트가 4개이면 -> 5개가 연속이면 1을 반환
			}
			if(strcmp(mok[i][j*2-1] , "X") == 0){	// O 판별과 동일
				x_checkpoint_v = i;
				x_checkpoint_h = j*2-1;
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
					return 2;	// x가 연속 5개이면 2를 반환
			}
		}
	}
	return 0;
}
void drawmok(char mok[20][38][4]){	// 오목판을 그릴 함수 drawmok
	int num = 0;		// 오목판의 좌표계를 표시해줄 숫자 num
	char num2 = 'A';	// 오목판의 좌표계를 표시해줄 문자 num2
	char buf[256];		// 오목판에 표시할 문자열을 저장할 buf
	for(int i=0;i<20;i++){	// 0~19 까지 반복
		sprintf(buf,"%2d",num);	// buf에 %2d 형식으로 num을 저장
		strcpy(mok[i][0] , buf);	// 오목판의 첫 열을 buf로 설정
		num++;
		// strcpy(mok[i][0] , "");
	}
	num = 0;	// num을 0으로 초기화
	for(int i=0;i<20;i++){	// 위와 같은 방법으로 첫 행에 번호를 설정
		if(i < 10 ){
			sprintf(buf,"%d ",num);
			strcpy(mok[0][i] , buf);
			num++;
		}
		else{				// 10보다 커지면 알파벳으로 설정
			sprintf(buf,"%c ",num2);
			strcpy(mok[0][i] , buf);
			num2++;
		}
		// strcpy(mok[0][i] , "");
	}
	strcpy(mok[1][1] ,"┌");		// 각 모서리 설정
	strcpy(mok[1][37] ,"┐");
	strcpy(mok[19][1] , "└");
	strcpy(mok[19][37] , "┘");
	for(int i=1;i<20;i++){		// 오목판 그리기
		for(int j=1;j<38;j++){
			if(j%2 == 1){
				if(i == 1){
					if(j == 1 || j == 37)		// 첫행의 첫 열과 마지막열 제외
						continue;
					else
						strcpy(mok[i][j] , "┬");	// 첫 행의 나머지는 T자로 설정
				}
				else if(i == 19){
					if(j == 1 || j == 37)		// 마지막 행의 첫 열과 마지막 열 제외
						continue;
					else
						strcpy(mok[i][j] , "┴");	// 마지막 행의 나머지는 ┴문자로 설정
				}
				else if(j == 1){				// 첫 열, 위와 동일
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
					strcpy(mok[i][j] , "┼");	// 나머지의 경우 +문자로 설정
				}
			}
			else{
				strcpy(mok[i][j], "─");		// 비율이 맞지 많아, 정사각형으로 출력하기 위해 - 문자 추가
			}
		}
	}
}

int main(void){
	int x,y;	// 좌표를 입력받을 x, y 선언
	fp = fopen(".server_log.log", "a");
	int cx,cy;	// client의 좌표 cx, cy 선언
	int count = 0;	// 턴 수를 셀 count 선언
	int gameset;	// 게임의 상태를 지정할 gameset 선언
	int num = 0;	// ?? 왜 있을까요 ㅋㅋㅋㅋ
	key_t key;		// 서버의 메시지 큐의 key 선언
    key_t key2;		// 클라이언트의 메시지 큐의 key2 선언
	key_t key3;		// 클라이언트의 시그널 메시지 큐의 key3 선언
	key = 2015;		// key 값 지정
    key2 = 11053;	// key2 값 지정
	key3 = 01010011;	// key3 값 지정
	msgid = msgget(key, IPC_CREAT|0644);	// 메시지 큐 생성
    mesgid = msgget(key2, IPC_CREAT|0644);	// 메시지 큐 생성2
	sigid = msgget(key3, IPC_CREAT|0644);	// 메시지 큐 생성3
    inmsg.mtype = 2;	// 수신 메시지의 mtype을 2로 설정
	mesg.mtype = 1;		// 송신 메시지의 mtye을 1로 설정
	int len;			// 메시지를 수신할 변수 len 설정
	int scannum = 0;	// 사용자로부터 입력받은 좌표의 개수를 판별할 변수 선언
	char buf[256];		// 메시지를 저장할 buf 선언
	char mok[20][38][4] = {"",};	// 오목판 배열 선언, 2바이트 특수문자를 수용하기 위해 3차원 배열로 선언
	drawmok(mok);		// 빈 오목판을 그릴 drawmok함수 호출
	bool turn = true;	// 자신의 턴임을 설정할 bool 선언
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


	strcpy(mesg.mtext, "Start");	// Start 메시지 생성
	msgsnd(msgid, (void *)&mesg, 80, 0);	// 메시지 전송
	printf("상대를 기다리고 있습니다...\n");
	len = msgrcv(mesgid, &inmsg, 80, 0, 0);	// 클라이언트로부터 연결을 대기
	if(strcmp(inmsg.mtext, "Connect") == 0){	// 클라이언트로부터 연결 메시지가 수신되면
		printf("상대방과 연결되었습니다.\n곧 게임이 시작됩니다.\n");	// 게임 시작
	}
	else{	// 연결 외 다른 메시지가 수신되면
		printf("연결에 오류가 발생하였습니다.\n프로그램을 종료합니다.\n");
		msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);
		msgctl(mesgid, IPC_RMID, (struct msqid_ds *)NULL);
		msgctl(sigid, IPC_RMID, (struct msqid_ds *)NULL);
		exit(1);	// 메시지 큐 제거 후 프로그램 종료
	}
		time(&t);	// 시간 구조체 t에 현재 시간을 입력
	starttime = (int)t;	// starttime에 현재 시간(초)를 저장
	tm = localtime(&t);	// 현재 시간을 localtime으로 받아 tm 구조체에 저장
	fprintf(fp,"시작 시간 : %04d/%02d/%02d %02d:%02d:%02d\n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	// local 타임을 이용하여 년/월/일 시:분:초 형식으로 게임 시작 시간을 로그파일에 기록
	int stopsig = msgrcv(sigid, &sigmsg, 80, 0, IPC_NOWAIT); // 클라이언트 강제 종료 시그널을 받음, 메시지가 없으면 넘어감
	do{	// 반복문 시작
		if(turn){	// 자신의 턴이면
			do{
				alarm(10);	// 알람을 10초로 설정, 10초 후 alarmhandler 실행
				printf("좌표를 입력하세요 : ");
				scannum = scanf("%d %d", &y, &x);	// 좌표를 입력받아 성공적으로 입력받은 개수를 scannum에 저장
				// 열이 x, 행이 y이므로 반대로 입력받아 저장함
				while (getchar() != '\n');	// 버퍼 삭제
			}while(scannum != 2);	// 좌표가 숫자 2개로 받을 동안 반복하여 좌표를 입력받음
			alarm(3600);	// 알람을 3600초로 설정, alarm은 시간을 재설정 시 기존의 alarm을 대체하므로
							// 10초 내에 입력을 하였다면 상대방의 입력을 기다린다.
							// 다시 자신의 턴이 돌아오면 제한시간은 10초로 재설정된다.
		}
		else{	// 자신의 턴이 아니면
			stopsig = msgrcv(sigid, &sigmsg, 80, 0, 0);	// 시그널 대기, 종료가 아니면 "go"를 받음
			if(strcmp(sigmsg.mtext, "QUIT") == 0){	// 강제 종료 시그널이 수신되면
				printf("상대방이 게임을 종료하였습니다.\n");
				printf("몰수승으로 처리됩니다.\n");
				time(&t);		// 시간 구조체 t에 현재 시간을 저장
				int endtime = (int)t - starttime;	// 현재시간에서 시작 시간을 빼고 endtime에 저장
				fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 진행된 시간을 분,초 형식으로 로그파일에 기록
				fprintf(fp, "승리\t(몰수승)\n\n");	// "승리" 문자열을 로그에 기록
				sleep(1);	// 1초간 정지
				break;		// 반복문 종료
			}
			len = msgrcv(mesgid, &inmsg, 80, 0, 0);	// 클라이언트 메시지를 수신
			char *string = inmsg.mtext;	// 그 내용을 string에 저장
			cx = atoi(string);			// atoi로 받아온 좌표를 정수형으로 변환하여 저장
			len = msgrcv(mesgid, &inmsg, 80, 0, 0);	// 위와 동일
			string = inmsg.mtext;		// x좌표와 y좌표로 2번 받기 때문에 2번 실행하였음
			cy = atoi(string);
		}
		if(x > 19 || y > 19 || x < 1 || y < 1){	// 입력받은 좌표가 범위를 벗어나면
			printf("1~19의 수만 입력해주세요.\n");
			continue;	// 반복문 다시 실행
		}
		if(turn && strcmp(mok[x][y*2-1], "O") == 0){	// 입력받은 좌표에 이미 돌이 있으면
			printf("돌이 이미 있습니다.\n");		
			continue;									// 반복문 다시 실행
		}
		if(turn && strcmp(mok[x][y*2-1] , "X") == 0 ){	// 위와 동일
			printf("돌이 이미 있습니다.\n");
			continue;
		}
		if(!turn && strcmp(mok[cx][cy*2-1], "O") == 0){	// 클라이언트가 보낸 좌표에 돌이 있으면
			strcpy(mesg.mtext, "exist");				// exist 메시지 전송
			msgsnd(msgid, (void *)&mesg, 80, 0);
			continue;									// 반복문 다시 실행
		}
		if(!turn && strcmp(mok[cx][cy*2-1] , "X") == 0 ){	// 위와 동일
			strcpy(mesg.mtext, "exist");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			continue;
		}
		if(turn){							// 성공적으로 좌표가 입력되면
			strcpy(mok[x][y*2-1] , "O");	// 해당 좌표의 내용을 돌로 바꿈
			strcpy(mesg.mtext, "complete");	// 클라이언트로 complete 메시지 전송 -> 턴을 변경하기 위함
			msgsnd(msgid, (void *)&mesg, 80, 0);	
			turn = !turn;	// 턴 종료
		}
		else{
			strcpy(mok[cx][cy*2-1] , "X");	// 위와 동일
			strcpy(mesg.mtext, "complete");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			turn = !turn;
		}
		system("clear");	// 화면 refresh
		printMok(mok);		// 오목판 출력
		strcpy(mesg.mtext, "EM");	// 출력 종료를 알리는 EM 메시지 전송
		msgsnd(msgid, (void *)&mesg, 80, 0);
		gameset = check(mok);	// 게임 상태 체크
		if(gameset == 1){		// 체크 결과가 1이면
			strcpy(mesg.mtext, "OWIN");	// server의 승리 메시지를 전송
			msgsnd(msgid, (void *)&mesg, 80, 0);
			time(&t);		// 시간 구조체 t에 현재 시간을 저장
			int endtime = (int)t - starttime;	// 현재시간에서 시작 시간을 빼고 endtime에 저장
			fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 진행된 시간을 분,초 형식으로 로그파일에 기록
			printf("승리!\n");						
			fprintf(fp, "승리\n\n");				// 승리를 로그에 기록
			sleep(1);							// 클라이언트의 처리를 위한 sleep(1)
			break;								// 반복문 종료
		}
		else if(gameset == 2){					// 위와 동일
			strcpy(mesg.mtext, "XWIN");
			msgsnd(msgid, (void *)&mesg, 80, 0);
			time(&t);		// 시간 구조체 t에 현재 시간을 저장
			int endtime = (int)t - starttime;	// 현재시간에서 시작 시간을 빼고 endtime에 저장
			fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 진행된 시간을 분,초 형식으로 로그파일에 기록
			printf("패배!\n");
			fprintf(fp,"패배\n\n");
			sleep(1);
			break;
		}
		else if(gameset == 4){					// 상대가 강제종료 했을 시 몰수승 처리
			printf("상대방이 게임을 종료하였습니다.\n");
			printf("몰수승으로 처리됩니다.\n");
			time(&t);		// 시간 구조체 t에 현재 시간을 저장
			int endtime = (int)t - starttime;	// 현재시간에서 시작 시간을 빼고 endtime에 저장
			fprintf(fp, "게임 시간 : %02d분%02d초\n", endtime/60, endtime%60);	// 진행된 시간을 분,초 형식으로 로그파일에 기록
			fprintf(fp, "승리\t(몰수승)\n");
			sleep(1);
			break;
		}
		else{
			strcpy(mesg.mtext, "Notyet");	// game이 끝나지 않으면 Notyet 메시지 전송
			msgsnd(msgid, (void *)&mesg, 80, 0);
		}
		count ++;		// 카운트 증가
	}while(count<(361/2));	// 카운트가 오목판의 좌표계의 절반보다 작을때 까지
							// -> 좌표계가 꽉 차면 무승부
	fclose(fp);				// 파일 fp를 닫음
	msgctl(msgid, IPC_RMID, (struct msqid_ds *)NULL);	// 메시지큐 삭제
	msgctl(mesgid, IPC_RMID, (struct msqid_ds *)NULL);	// 메시지큐 삭제2
	msgctl(sigid, IPC_RMID, (struct msqid_ds *)NULL);	// 메시지큐 삭제3
	return 0;
}

