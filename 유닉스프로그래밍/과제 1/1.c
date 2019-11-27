#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	int fd,wfd;
	int n,c;
	char buf[10];
	extern char *optarg;
	extern int optind;

	//while((n=getopt(argc, argv, "AbeEnstTuv:")) != -1){
	//	switch(n){
	//		case A:
	//			printf("option : A");
	//			break;
	//	}
	//}
	if((fd = open(argv[1], O_RDONLY)) == -1){
		perror("Open");
		exit(1);
	}
	if((wfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644)) == -1){
		perror("Open2");
		exit(1);
	}

	while((c = read(fd,buf,sizeof(char)*2))>0)
		if(write(wfd,buf,c) != c)
			perror("Write");
	if(c == -1)
		perror("Read");
	close(fd);
	close(wfd);
	return 0;
}

