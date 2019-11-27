#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
	struct stat buf;

	stat("1.txt", &buf);
	printf("Link Count before link()=%d\n", (int)buf.st_nlink);

	link("1.txt", "1.ln");
	stat("1.txt", &buf);
	printf("Link Count after link()=%d\n", (int)buf.st_nlink);

	stat("1.ln", &buf);
	printf("Link Count of 1.ln=%d\n", (int)buf.st_nlink);

	symlink("1.txt", "1.sln");

	stat("1.txt", &buf);
	printf("Link Count of 1.txt=%d\n", (int)buf.st_nlink);

	stat("1.sln", &buf);
	printf("Link Count of 1.sln=%d\n", (int)buf.st_nlink);
	
	lstat("1.sln", &buf);
	printf("Link Count of 1.sln=%d\n", (int)buf.st_nlink);

	return 0;
}
