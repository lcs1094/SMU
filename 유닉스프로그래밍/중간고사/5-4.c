#include <time.h>
#include <stdio.h>


int main(void){
	struct tm *tm;
	int n;
	time_t t;
	char buf[257];

	char dayy[100][100] = {{"(일)"},{"(월)"},{"(화)"},{"(수)"},{"(목)"},{"(금)"},{"(토)"}};
	time(&t);
	tm = localtime(&t);
	printf("%d. %d. %d. %s %d:%d:%d KST\n", tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,dayy[tm->tm_wday],tm->tm_hour,tm->tm_min,tm->tm_sec);

	return 0;
}
