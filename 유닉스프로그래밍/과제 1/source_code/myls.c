#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
bool a = false;	// 여러 함수에서 사용하므로 전역변수로 선언
bool i = false;
bool l = false;
bool d = false;
bool exist = false;	// 마지막 인자가 옵션이 아닌 대상 파일인지 확인하는 bool
bool dirprint = false;	// 디렉터리를 표시해야 하는지에 대한 bool
bool s = false;

void mode_to_rwx(struct stat buf){		// mode를 rwx식으로 변경해주는 함수
	char mod[10];						// 10글자이기 때문에 10글자 mod 배열 선언
	if(S_ISDIR(buf.st_mode))			// 인자가 디렉터리면
		mod[0] = 'd';					// 첫 글자는 d
	else if(S_ISREG(buf.st_mode))		// 인자가 일반파일이면
		mod[0] = '-';					// 첫 글자는 -
	else if(S_ISCHR(buf.st_mode))		// 인자가 문자입력파일이면
		mod[0] = 'c';					// 첫 글자는 c
	else if(S_ISBLK(buf.st_mode))		// 인자가 블록파일이면
		mod[0] = 'b';					// 첫 글자는 b
	else if(S_ISLNK(buf.st_mode))		// 인자가 심볼릭링크파일이면
		mod[0] = 'l';					// 첫 글자는 l
	else if(S_ISSOCK(buf.st_mode))		// 인자가 소켓파일이면
		mod[0] = 's';					// 첫 글자는 s
	else if(S_ISFIFO(buf.st_mode))		// 인자가 fifo파일이면
		mod[0] = 'f';					// 첫 글자는 f
	if(buf.st_mode & S_IREAD)			// 사용자 r 권한이 있으면
		mod[1] = 'r';					// 글자는 r
	else								// 해당 권한이 없으면
		mod[1] = '-';					// 글자는 -
	if(buf.st_mode & S_IWRITE)			// 사용자 w 권한이 있으면
		mod[2] = 'w';					// 글자는 w
	else								// 해당 권한이 없으면
		mod[2] = '-';					// 글자는 -
	if(buf.st_mode & S_IEXEC)			// 사용자 x 권한이 있으면
		mod[3] = 'x';					// 글자는 x
	else								// 해당 권한이 없으면
		mod[3] = '-';					// 글자는 -
	if(buf.st_mode & (S_IREAD >> 3))	// group r 권한이 있으면
		mod[4] = 'r';					// 글자는 r
	else								// 해당 권한이 없으면
		mod[4] = '-';					// 글자는 -
	if(buf.st_mode & (S_IWRITE >> 3))	// group w 권한이 있으면
		mod[5] = 'w';					// 글자는 w
	else								// 해당 권한이 없으면
		mod[5] = '-';					// 글자는 -
	if(buf.st_mode & (S_IEXEC >> 3))	// group x 권한이 있으면
		mod[6] = 'x';					// 글자는 x
	else								// 해당 권한이 없으면
		mod[6] = '-';					// 글자는 -
	if(buf.st_mode & (S_IREAD >> 6))	// other r 권한이 있으면
		mod[7] = 'r';					// 글자는 r
	else								// 해당 권한이 없으면
		mod[7] = '-';					// 글자는 -
	if(buf.st_mode & (S_IWRITE >> 6))	// other w 권한이 있으면
		mod[8] = 'w';					// 글자는 w
	else								// 해당 권한이 없으면
		mod[8] = '-';					// 글자는 -
	if(buf.st_mode & (S_IEXEC >> 6))	// other x 권한이 있으면
		mod[9] = 'x';					// 글자는 x
	else								// 해당 권한이 없으면
		mod[9] = '-';					// 글자는 -
	printf("%s ", mod);					// mod 문자열 출력

	return;								// 함수 종료
}
void printfile(char *target){			// 파일의 정보를 출력해주는 함수
	struct stat bufs;					// 정보를 저장할 stat 구조체 bufs
	struct passwd *pw;					// passwd 구조체 pw
	struct tm *tm;						// 시간 구조체 tm
	stat(target, &bufs);				// 인자의 정보를 bufs에 저장
		if(!a){							// a옵션이 아니면
			if(target[0] == '.')		// 인자의 첫글자가 .이면
				return ;				// 함수 종료
		}
		if(i)							// i옵션이면
			printf("%d ",(int)bufs.st_ino);	// 인자의 inod 출력
		if(s){							// s옵션이면
			printf("%d ", (int)bufs.st_blocks/2);	// 인자의 block수를 2로 나누어 출력
		}
		if(l){							// l옵션이면
			mode_to_rwx(bufs);			// bufs 구조체를 인자로 mode_to_rwx 함수 실행
			printf("%2d ", (unsigned int)bufs.st_nlink);	// 인자의 링크 수 출력
			pw = getpwuid(bufs.st_uid);		// 인자의 uid로 getpwuid함수를 실행하여 pw에 저장
			printf("%s ", pw->pw_name);		// 해당 uid를 갖는 소유자의 이름을 출력
			pw = getpwuid(bufs.st_gid);		// 인자의 gid로 getpwuid함수를 실행하여 pw에 저장
			printf("%s ", pw->pw_name);		// 해당 gid를 갖는 소유자의 이름을 출력 
			printf("%6ld ", bufs.st_size);	// 인자의 size를 출력
			tm = localtime(&bufs.st_mtime);	// 인자의 modified time을 인자로 localtime을 적용
			printf("%02d월 %02d %02d:%02d ", tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);	// 시간 출력
		}
	printf("%s\n", target);	// 인자의 이름을 출력
	return ;
}
void printdir(char *target){	// 디렉터리를 탐색하는 함수
	DIR *dp;					// 디렉터리 구조체 dp 선언
	struct dirent *dent;		// dirent 구조체 dent 선언
	dp = opendir(target);		// dp는 인자 이름을 가지는 디렉터리
	while((dent = readdir(dp))){	// 디렉터리의 파일들을 순차적으로 탐색
		printfile(dent->d_name);	// 디렉터리의 파일 이름을 인자로 printfile 함수 실행
	}
	dirprint = true;			// 디렉터리를 출력했다면 dirprint 변수를 true로 설정
	closedir(dp);				// 디렉터리 닫음
	return ;
}
int main(int argc, char *argv[]){
	int n;	// 옵션을 저장할 n 선언
	char wd[BUFSIZ];	// 현재 디렉터리를 저장할 wd char배열 선언
	struct stat buf;	// 읽어올 파일의 정보를 저장할 buf 선언
	extern char *optarg;
	extern int optind;


	while((n=getopt(argc, argv, "ailds")) != -1){	// 옵션 인자를 읽어들임
		switch(n){			// 해당 옵션이 있으면 그 이름을 같은 bool 변수를 true로 설정
			case 'a':
				a = true;
				break;
			case 'i':
				i = true;
				break;
			case 'l':
				l = true;
				break;
			case 'd':
				d = true;
				break;
			case 's':
				s = true;
				break;

		}
	}
	char last[30];			// 인자의 이름을 저장할 char 배열 last 선언
	strcpy(last, argv[argc-1]);	// last에 마지막 인자의 이름을 저장
	if(last[0] != '-' && argv[argc-1] != argv[0])	// 마지막 인자가 '-'로 시작하거나 실행파일 외의 인자가 없으면 -> -는 옵션으로 사용되어 파일이름의 시작으로 사용할 수 없음
		exist = true;	// exist = true

	
	if(exist){	// 마지막 인자가 파일이름이면 -> 파일 이름 인자가 없으면 현재 디렉터리를 인자로 한다.
		for(int i=1;i<argc;i++){	// 입력받은 인자의 수만큼 i의 값을 증가
			strcpy(last, argv[i]);	// last에 인자의 이름을 순차적으로 저장
			if(last[0] != '-'){		// 인자의 이름이 '-'로 시작하지 아니고( *옵션이 아니면) i가 0보다 크면(타켓파일이 있을 때만 실행하기 위해)
				if((stat(argv[i], &buf)) == -1){	// 마지막 인자의 정보를 buf에 저장하는데 실패하면
					printf("ls: '%s'에 접근할 수 없습니다: 그런 파일이나 디렉터리가 없습니다\n", argv[argc-1]);	// 에러 메시지 출력
					continue;
				}
				if(S_ISDIR(buf.st_mode) && !d){			// 받은 인자가 디렉터리이면서 d옵션이 사용되지 않으면
					if(dirprint)	// 이미 printdir이 실행이 되었다면 *다중 인자 처리 시 
						printf("\n%s:\n", argv[i]);	// 디렉터리의 이름을 출력
					printdir(argv[i]);	// 해당 인자의 디렉터리를 dp로 연다.
				}
				else if(S_ISDIR(buf.st_mode) && d){	// 인자가 디렉터리이고 d 플래그가 true이면
					printfile(argv[i]);	// 해당 디렉터리의 정보 출력
				}
				else{							// 타겟파일이 디렉터리가 아니면
					printfile(argv[i]);
					//printf("%s\n", argv[i]);	// 위에서 파일의 존재 여부는 체크하였기 때문에 파일의 이름을 출력한다.
				}
			}
		}
	}
	else{		// 타겟 파일이 없으면
		getcwd(wd, BUFSIZ);		// 현재 디렉터리를 wd에 저장
		if(d){					// d 옵션이 켜져 있으면
			printfile(wd);		// 현재 디렉터리의 정보를 출력
		}
		else{					// d 옵션이 꺼져 있으면
		printdir(wd);			// 현재 디렉터리를 printdir인자로 함수 실행
		}
	}
	return 0;
}