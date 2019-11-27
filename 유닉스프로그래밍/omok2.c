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

void printMok(char mok[20][20]);
int check(char mok[20][20]);

void printMok(char mok[20][20]){
	for(int i=0;i<20;i++){
		for(int j=0;j<20;j++)
			if(i == 0){
				printf("%d\t", mok[i][j]);
			}
			else if(j == 0)
				printf("%d\t", mok[i][j]);
			else
				printf("%c\t", mok[i][j]);
		printf("\n\n");
	}
}
int check(char mok[20][20]){	// 1 : o승 2 : x승 3: 무승부
	int h_o_count = 0;	// 가로
	int h_x_count = 0;
	int v_o_count = 0;	// 세로
	int v_x_count = 0;
	int d_o_count = 0;	// 오른쪽 아래 대각선
	int d_x_count = 0;
	int d2_o_count = 0;	// 왼쪽 아래 대각선
	int d2_x_count = 0;
	int i = 0, j = 0;
	// bool h_o_check = false;
	// bool h_x_check = false;
	// bool v_o_check = false;
	// bool v_x_check = false;
	// bool d_o_check = false;
	// bool d_x_check = false;
	// bool d2_o_check = false;
	// bool d2_x_check = false;
	int o_checkpoint_v = 21;
	int o_checkpoint_h = 21;
	int x_checkpoint_v = 21;
	int x_checkpoint_h = 21;
	int ii,jj;
	for(i=1;i<20;i++){
		for(j=1;j<20;j++){
			if(mok[i][j] == 'o'){
				o_checkpoint_v = i;
				o_checkpoint_h = j;
				if(i<15 && j<15){
					v_o_count = 0;
					d_o_count = 0;
					d2_o_count = 0;
					for(ii=1;ii<5;ii++){
						h_o_count = 0;
						if(mok[o_checkpoint_v+ii][o_checkpoint_h] == 'o')
								v_o_count++;
						for(jj=1;jj<5;jj++){
							if(mok[o_checkpoint_v][o_checkpoint_h+jj] == 'o')
								h_o_count++;
							if(ii == jj && mok[o_checkpoint_v+ii][o_checkpoint_h+jj] == 'o')
								d_o_count++;
							if(ii == jj && mok[o_checkpoint_v+ii][o_checkpoint_h-jj] == 'o')
								d2_o_count++;
						}
					}
					if(h_o_count == 4 || v_o_count == 4 || d_o_count == 4 || d2_o_count == 4)
						return 1;
				}
			}
			if(mok[i][j] == 'x'){
				x_checkpoint_v = i;
				x_checkpoint_h = j;
				if(i<15 && j<15){
					v_x_count = 0;
					d_x_count = 0;
					d2_x_count = 0;
					for(ii=1;ii<5;ii++){
						h_x_count = 0;
						if(mok[x_checkpoint_v+ii][x_checkpoint_h] == 'x')
								v_x_count++;
						for(jj=1;jj<5;jj++){
							if(mok[x_checkpoint_v][x_checkpoint_h+jj] == 'x')
								h_x_count++;
							if(ii == jj && mok[x_checkpoint_v+ii][x_checkpoint_h+jj] == 'x')
								d_x_count++;
							if(ii == jj && mok[x_checkpoint_v+ii][x_checkpoint_h-jj] == 'x')
								d2_x_count++;
						}
					}
					if(h_x_count == 4 || v_x_count == 4 || d_x_count == 4 || d2_x_count == 4)
						return 2;
				}
			}
		}
	}
	return 0;
}
int main(void){
	int x,y;
	int count = 0;
	int gameset;
	int num = 0;
	char mok[20][20] = {"",};
	for(int i=0;i<20;i++){
		mok[i][0] = num;
		num++;
	}
	num = 0;
	for(int i=0;i<20;i++){
		mok[0][i] = num;
		num++;
	}
	bool turn = true;
	do{
		srand(time(NULL));
		x = rand()%7+1;
		sleep(1);
		y = rand()%7+1;
		//scanf("%d %d", &x, &y);
		if(x > 20 || y > 20 || x < 1 || y < 1){
			printf("1~20의 수만 입력해주세요.\n");
			continue;
		}
		if(mok[x][y] == 'o' || mok[x][y] == 'x'){
			printf("돌이 이미 있습니다.\n");
			continue;
		}
		if(turn){
			mok[x][y] = 'o';
			turn = !turn;
		}
		else{
			mok[x][y] = 'x';
			turn = !turn;
		}
		system("clear");
		printMok(mok);
		gameset = check(mok);
		if(gameset == 1){
			printf("o 승리!\n");
			break;
		}
		else if(gameset == 2){
			printf("x 승리!\n");
			break;
		}
		count ++;
	}while(count<(361/2));
	return 0;
}

