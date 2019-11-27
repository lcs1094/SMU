#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

int main(int argc, char *argv[]){
	FILE *fd, *wfd, *tfd;	// 읽어올 파일 fd, 작성할 파일 wfd, 파일 확인용 tfd 파일 구조체
	int n,c;				// 옵션을 저장할 n, fgetc를 받을 c 선언
	char answer[BUFSIZ];	// i옵션 사용시 답변을 저장할 answer 선언
	struct stat buf;		// 파일의 정보를 저장할 stat 구조체 buf 선언
	extern char *optarg;
	extern int optind;
	bool l = false;			// 옵션 l flag
	bool s = false;			// 옵션 s flag
	bool i = false;			// 옵션 i flag
	bool exist = false;		// i옵션 사용시 파일의 존재를 확인할 bool exist

	while((n=getopt(argc, argv, "lsi")) != -1){
		switch(n){		// 입력된 옵션으로 switch문 실행
			case 'l':	// l옵션이면
				l = true;	// l = true;
				break;
			case 's':	// s옵션이면
				s = true;	// s = true;
				break;
			case 'i':	// i옵션이면
				i = true;	//i = true;
				break;
		}
	}
	if(argc < 3){	// 인자가 3개 보다 적으면, *첫번째 인자는 ./mycp
		printf("cp: '%s' 다음에 대상 파일 명령이 누락됨\n", argv[argc-1]);	// cp와 동일한 에러 메시지 출력
		printf("Try 'cp --help' for more information.\n");			// cp와 동일한 에러 메시지 출력
		exit(1);								// 프로그램 종료
	}
	if((fd = fopen(argv[argc-2], "r")) == NULL){	// 복사할 대상 파일이 없으면
		printf("%s 파일이 없습니다.", argv[argc-2]);	// 에러 출력
		exit(1);	// 프로그램 종료
	}
	if(l){	// l 옵션을 사용했을 경우
			link(argv[argc-2],argv[argc-1]);	// 복사할 대상 파일을 두번째 인자의 이름의 링크 파일로 생성
			exit(1);								// 반복문 종료
	}
	if(s){	// s옵션을 사용했을 경우
		symlink(argv[argc-2],argv[argc-1]);	// 복사할 대상 파일을 두번째 파일 인자의 이름의 심볼릭 링크 파일로 생성
		exit(1);								// 반복문 종료
	}
	if(i){	// i옵션을 사용했을 경우
		if((tfd = fopen(argv[argc-1], "r")) != NULL){	// 복사할 대상을 tfd로 읽기에 성공하면 -> 파일이 존재하면
			exist = true;	// exist = true;
			fclose(tfd);	// tfd 파일 종료
		}
	}
	if(exist){	// i옵션을 사용하였고, 파일이 이미 존재할 경우
		printf("cp : '%s'를 덮어쓸까요? ", argv[argc-1]);	// 덮어쓸지 물어봄
		scanf("%s", answer);	// 답변을 입력받음
		if(answer[0] != 'y' && answer[0] != 'Y')	// 답변이 y, Y로 시작하지 않으면
			exit(1);								// 프로그램 종료
		else{										// 답변이 y,Y로 시작하면
			exist = false;							// exist = false -> 첫 입력 후 실행하지 않기 위해서
		}	
	}
	if((wfd = fopen(argv[argc-1], "w")) == NULL){	// 복사 타겟 파일을 여는데 실패하면
		perror("Create File");						// 에러 출력
		exit(1);									// 프로그램 종료
	}
	stat(argv[argc-2], &buf);		// 첫번째 파일명 인자의 정보를 buf에 저장
	while(( c = fgetc(fd)) != EOF){	// 첫번째 파일 인자의 끝까지 한 글자씩 c에 저장한다.
		fputc(c, wfd);		// 타겟 파일에 한 문자씩 입력
	}

	fclose(fd);				// fd 파일 종료
	fclose(wfd);			// wfd 파일 종료
	return 0;
}

