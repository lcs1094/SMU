#include <stdio.h>

int main(int argc, char *argv[])
{
	int n,i, sum=0, mul=1;
	extern char *optarg;
	int count =0;	// 숫자 인자의 위치를 표시할 변수
	for(int i=0;i<argc;i++){	// 입력된 입자를 검사
		if(argv[i][0] >='0' && argv[i][0]<='9'){ // 인자가 숫자이면
			count = i; // count는 해당 인자의 위치를 갖는다.
			break;	// 숫자 인자의 위치를 파악했으므로
				// 반복문을 종료한다.
		}
	}
	while((n=getopt(argc, argv, "am:"))!=-1){	// am옵션이 있는 동안 반복
		switch(n){	// 옵션에 따라서 switch문 실행
		case 'm':
			for(i=count;i<argc;i++)	// 숫자 인자들을 순차적으로 i가 갖는다.
				mul*=atoi(argv[i]); // 인자들을 곱셈
			printf("%d\n", mul);
			break;
		case 'a':
			for(i=count;i<argc;i++) // 위의 과정과 동일
				sum+=atoi(argv[i]); // 인자들을 덧셈
			printf("%d\n", sum);
			break;
		default:
			printf("Usage: 1-8 [-am] n1 n2 …\n"); //지정된 옵션이 아닐 경우의 처리
			break;
			}
	}
	return 0;
}
