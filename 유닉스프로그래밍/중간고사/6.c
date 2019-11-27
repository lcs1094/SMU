#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int rfd, n;
    int i;
    int linecount=0;
    int wordcount=0;
    int charcount=0;
    char buf[10];

    rfd = open(argv[2], O_RDONLY);
    if(rfd == -1) {
	    perror("Open Source File");
	        exit(1);
    }
    while ((n = read(rfd, buf, 6)) > 0){
	for(i=0;i<sizeof(buf);i++){
		if(buf[i]='\0')
	    		linecount++;
		else if(buf[i]=' ')
    			wordcount++;
		else
			charcount++;

	}

    }

    while((n=getopt(argc, argv, "lwc:"))!=-1){ 
                switch(n){  // 옵션에 따라서 switch문 실행
                case 'l':
                        printf("%d\n", linecount);
                        break;
                case 'w':
                        printf("%d\n", wordcount);
                        break;
                case 'c':
                        printf("%d\n", charcount);
                        break;
                default:
                        break;
                        }
        }

    if (n == -1) perror("Read");


    close(rfd);

    return 0;
}    
