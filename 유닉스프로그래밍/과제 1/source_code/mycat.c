#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	FILE *fd;
	int opt,ch;
	struct stat buf;	// 디렉터리인지 판별하기 위한 구조체 선언
	int kind;			// 디렉터리인지 판결하기 위한 st_mode 사용을 위한 변수 선언
	extern char *optarg;
	extern int optind;
	bool A = false;		// A 옵션 판별 bool
	bool e = false;		// e 옵션 판별 bool
	bool E = false;		// E 옵션 판별 bool
	bool T = false;		// T 옵션 판별 bool
	bool t = false;		// t 옵션 판별 bool
	bool v = false;		// v 옵션 판별 bool
	bool n = false;		// n 옵션 판별 bool
	bool b = false;		// b 옵션 판별 bool
	bool s = false;		// s 옵션 판별 bool
	int line_num = 1;	// Line Number시 표시를 위한 변수
	bool endline = false;	// 개행문자를 판별하는 변수
	bool endline2 = true;	// 개행문자의 연속을 판별하는 변수
	long cur;				// cursor 변수

	while((opt=getopt(argc, argv, "AeETtvnbs")) != -1){
		switch(opt){		// 옵션이 입력되면 해당 옵션의 bool을 true로 변경
			case 'A':
				A = true;
				break;
			case 'e':
				e = true;
				break;
			case 'E':
				E = true;
				break;
			case 'T':
				T = true;
				break;
			case 't':
				t = true;
				break;
			case 'v':
				v = true;
				break;
			case 'n':
				n = true;
				break;
			case 'b':
				b = true;
				break;
			case 's':
				s = true;
				break;
		}
	}
	for(int i=1;i<argc;i++){	// 입력 받은 인자들의 수 많큼 i의 값을 변경하며 실행 
		char last[30];			// 인자의 이름을 저장할 char 배열 last 선언
		strcpy(last, argv[i]);	// last에 i번째 인자의 이름 저장
		if(last[0] != '-'){		// 인자의 이름이 '-'로 시작하지 않으면 *옵션이 아니면
			if((fd = fopen(argv[i], "r")) == NULL){	// 해당 인자를 fopen으로 열기
				printf("cat: %s: 그런 파일이나 디렉터리가 없습니다\n", argv[i]);	// 실패시 에러 메시지 출력
				continue;	// 다음 인자로 넘어감
			}
			stat(argv[i], &buf);	// 인자의 정보를 buf에 저장
			kind = buf.st_mode & S_IFMT;	// buf의 st_mode에서 지정한 값을 가지고 옴
			if(kind == S_IFDIR){			// kind의 값이 S_IFDIR, 즉 받은 인자가 디렉터리이면
				printf("cat: %s:디렉터리입니다\n", argv[i]);	// 에러 메시지 출력
				fclose(fd);			// 파일을 닫음
			}
		while((ch = fgetc(fd)) != EOF){	// 문자 처리를 원활히 하기 위해 fgetc 사용
			cur = ftell(fd);	// 커서를 파일의 커서 위치로
			if(E || A || e){	// e = vE, A = vET 옵션과 같으므로 동시 처리
				if(ch == 10)	// 읽은 문자가 개행문자라면
					printf("$");	// $ 출력
			}
			if(T || A || t){	// t = vT와 같은 옵션이므로 동시 처리
				if(ch == 9){	// 읽은 문자가 수평 탭이라면
					printf("^I");	// ^I 출력
					continue;	// 수평 탭은 출력하지 않고 다음 문자를 읽음
				}
			}
			if(v || e || t){	// 위의 옵션의 동일성 참조
				if(ch == 7 || ch == 8 || ch == 12 || ch == 13 || ch == 11){
					// 읽은 문자가 수직 탭, 경고음, 백스페이스 등의 특수 문자라면
					printf("^");	// ^출력
					continue;		// 해당 문자는 출력하지 않고 다음 문자를 읽음
				}
			}
			if(n){			// n옵션은 linenumber를 출력한다.
				if((int)cur == 1){	// 커서의 위치가 파일의 첫 문자라면
					printf("     %d	", line_num);	// 라인넘버 출력
					line_num++;						// 라인넘버 증가
				}
				if(endline){						// 이전에 읽은 문자가 개행문자라면
					printf("     %d	", line_num);	// 라인넘버 출력
					line_num++;						// 라인넘버 증가
					endline = false;				// endline = false
				}
				if(ch == 10){						// 읽은 문자가 개행문자이면
					endline = true;					// endline = true
				}
			}
			if(b){			// b옵션은 빈 줄을 제외하고 라인넘버 출력
				if((int)cur == 1){		// 커서의 위치가 파일의 첫 문자라면
					if(ch != 10){		// 읽은 문자가 개행이 아니면(개행이면 첫 줄은 공백)
						printf("     %d	", line_num);	// 라인넘버 출력
						line_num++;						// 라인넘버 증가
					}
				}
				if(endline){			// endline 이 true이면
					if(ch != 10){		// 읽은 문자가 개행이 아니면(개행이면 공백)
						printf("     %d	", line_num);	// 라인넘버 출력
						line_num++;						// 라인넘버 증가
						endline = false;				// endline = false
					}
				}
				if(ch == 10){			// 읽은 문자가 개행문자이면
					endline = true;		// endline = true;
				}
			}
			if(s){			// s옵션은 연속되는 여러 공백을 하나의 공백으로 표시
				if(endline){	// endline 이 true이면
					if(ch == 10 && endline2)	// 읽은 문자가 개행이고 endline2가 true이면
						continue;				// 출력하지 않고 다음 문자를 읽음
				}
				if(ch == 10){		// 읽은 문자가 개행문자이면
					if(endline)		// endlie이 true이면
						endline2 = true;	// endline2 = true
					// * endline2는 공백이 연속되는지 파악하기 위한 변수로,
					//	 endline이 읽은 문자의 바로 전 문자가 개행이라면,
					//	 endline2는 연속된 개행이면 true이다.
					endline = true;	// endline = true
				}
				else{		// 읽은 문자가 개행문자가 아니면
					endline2 = false;	// endline2 = false
					endline = false;	// endline = false
				}
			}	
			printf("%c", ch);	// 읽은 문자를 화면에 출력
		}
		fclose(fd);	// 파일 닫기
		}
	}
	return 0;
}

