/*
 Eescape maze game you can customize map if you want
 you can add monsters to map but if you add monster to map you have to create a new thread for that monster
 W-A-S-D for play if monsters catch you you will dead.
 if you can go * sign YOu WON THE GAME !!!
 HAVE FUN !
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <termios.h>
//set_mode method for keyboard capture
void set_mode(int want_key){
  static struct termios old,new;
  if(!want_key){
    tcsetattr(STDIN_FILENO, TCSANOW,&old);
    return;
  }
  tcgetattr(STDIN_FILENO,&old);
  new = old;
  new.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO,TCSANOW,&new);

}
//for getting key from keyboard
int get_key(){
  int c=0;
  struct timeval tv;
  fd_set fs;
  tv.tv_usec = tv.tv_sec = 0;
  FD_ZERO(&fs);
  FD_SET(STDIN_FILENO,&fs);
  select(STDIN_FILENO +1,&fs,0,0,&tv);

  if(FD_ISSET(STDIN_FILENO,&fs)){
    c = getchar();
    set_mode(0);
  }
  return c;
}



//Game Stuff

#define WALL 0
#define FLOOR 1
#define MONSTERUD 50
#define MONSTERRL 51
#define MONSTERSLEEPY 100
#define CHARACTER 99
#define EXIT 98

int outline[15][30] = {
{WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,CHARACTER,WALL},
{WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,MONSTERRL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,WALL,FLOOR,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,WALL,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,WALL,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,MONSTERUD,WALL,WALL,FLOOR,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,WALL,WALL,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,FLOOR,FLOOR,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,MONSTERSLEEPY,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,WALL,WALL,WALL,FLOOR,FLOOR,WALL,FLOOR,FLOOR,FLOOR,WALL,FLOOR,WALL},
{WALL,EXIT,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL} };



typedef struct {
	char type;
	int x, y;//location information

}GameObject;



GameObject objects[15][30];
GameObject e, c ,mUD,mRL,mS;
void loadMap(int[15][30]);
void drawMap();
void capture_signal(int);
void *start_monsterUD();
void *start_monsterRL();
void *start_game();
void *wake_up_monster();
void *controlMonster();

int wakeMonsterUp();

pthread_t monster_rl_thread,start_game_thread,monster_ud_thread;
pthread_t sleepy_monster_thread;
pthread_t control;

int main() {


	signal(SIGINT, capture_signal);


	loadMap(outline);
	pthread_create(&monster_rl_thread,NULL,start_monsterRL,NULL);
	pthread_create(&start_game_thread,NULL,start_game,NULL);
	pthread_create(&monster_ud_thread,NULL,start_monsterUD,NULL);
	pthread_create(&control,NULL,controlMonster,NULL);
	pthread_join(start_game_thread,NULL);
	pthread_join(monster_rl_thread,NULL);
	pthread_join(monster_ud_thread,NULL);
	pthread_join(control,NULL);

	//join sleepy monster thread when character close to him


	return 0;
}

void loadMap(int outline[15][30]) {
	GameObject wall, floor, exit, character,monsterUD,monsterRL,monsterSleepy;

	wall.type = 'X';
	floor.type = ' ';
	exit.type = '*';
	character.type = '@';
	monsterUD.type = 'M';
	monsterRL.type = 'M';
	monsterSleepy.type = 'W';


	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 30; y++) {
			switch (outline[x][y]) {
			case WALL:
				wall.x = x; wall.y = y;
				objects[x][y] = wall;
				break;
			case FLOOR:
				floor.x = x; floor.y = y;
				objects[x][y] = floor;
				break;
			case CHARACTER:
				character.x = x; character.y = y;
				c = character;
				objects[x][y] = character;
				break;
			case EXIT:
				exit.x = x; exit.y = y;
				objects[x][y] = exit;
				break;
				case MONSTERUD:
				monsterUD.x = x; monsterUD.y = y;
				mUD = monsterUD;
				objects[x][y] = monsterUD;
				break;
				case MONSTERRL:
				monsterRL.x = x; monsterRL.y = y;
				mRL = monsterRL;
				objects[x][y] = monsterRL;
				break;
				case MONSTERSLEEPY:
				monsterSleepy.x = x; monsterSleepy.y = y;
				mS = monsterSleepy;
				objects[x][y] = monsterSleepy;
				break;


			}
		}
	}
}
void drawMap() {

	for (int x = 0; x < 15; x++) {
		for (int y = 0; y < 30; y++) {
			printf("%c ", objects[x][y].type);
		}
		printf("\n");
	}
}
void capture_signal(int signo) {
	if (SIGINT == signo)
		exit(1);
}
void *start_game(){
	int choice;
	while (1) {
		drawMap();

		//printf("\n\n\n Please pick right or left for escaping maze(CTRL+C for quit)");
		set_mode(1);

		while(!(choice=get_key())) usleep(10000);

	//	scanf("%d", &choice);
		if(choice == 115){
			//move right
			if (outline[c.x + 1][c.y] == EXIT) {
				printf("Congratulations you find the exit !!! \n");
				exit(1);
				break;
			}
			if (outline[c.x + 1][c.y] == FLOOR) {
				outline[c.x][c.y] = FLOOR;
				outline[c.x + 1][c.y] = CHARACTER;
				loadMap(outline);
			}
			else continue;
			//move right
		}
		else if (choice == 119) {
			//move left
			if (outline[c.x -1][c.y] == EXIT) {
				printf("Congratulations you find the exit !!! \n");
				exit(1);
				break;
			}
			if (outline[c.x - 1][c.y] == FLOOR) {
				outline[c.x][c.y] = FLOOR;
				outline[c.x -1][c.y] = CHARACTER;
				loadMap(outline);
			}
			else continue;
			//move left
		}
		else if (choice == 100) {
			//move down
			if (outline[c.x][c.y+1] == EXIT) {
				printf("Congratulations you find the exit !!! \n");
				exit(1);
				break;
			}
			if (outline[c.x][c.y+1] == FLOOR) {
				outline[c.x][c.y] = FLOOR;
				outline[c.x][c.y+1] = CHARACTER;
				loadMap(outline);
			}
			else continue;
			//move down
		}
		else if (choice == 97) {
			//move up
			if (outline[c.x][c.y -1] == EXIT) {
				printf("Congratulations you find the exit !!! \n");
				exit(1);
				break;
			}
			if (outline[c.x][c.y -1] == FLOOR) {
				outline[c.x][c.y] = FLOOR;
				outline[c.x][c.y - 1] = CHARACTER;
				loadMap(outline);
			}
			else continue;
			//move up
		}



	}
	return NULL;
}
void *start_monsterRL(){
  while(1){
		while(outline[mRL.x ][mRL.y+1]!=WALL){
			sleep(2);
			outline[mRL.x][mRL.y] = FLOOR;
			if(outline[mRL.x][mRL.y+1]==CHARACTER){
				//game over situation
				printf("Sorry you couldn't finish the map.\n Monster ate you!!!");
				exit(1);
			}
			outline[mRL.x][mRL.y+1] = MONSTERRL;
			loadMap(outline);
			drawMap();
		}while(outline[mRL.x][mRL.y-1]!=WALL){
			sleep(2);
			outline[mRL.x][mRL.y] = FLOOR;
			if(outline[mRL.x][mRL.y-1]==CHARACTER){
				//Game over situation
				printf("Sorry you couldn't finish the map.\n Monster ate you!!!");
				exit(1);

			}
			outline[mRL.x][mRL.y-1] = MONSTERRL;
			loadMap(outline);
			drawMap();

		}
	}
	return NULL;
}
void *start_monsterUD(){
	while(1){
		while(outline[mUD.x+1][mUD.y]!=WALL){
			sleep(2);
			outline[mUD.x][mUD.y] = FLOOR;
			if(outline[mUD.x+1][mUD.y]==CHARACTER){
				printf("Sorry you couldn't finish the map.\n Monster Ate You !!!");
				exit(1);
				//game over situation
			}
			outline[mUD.x+1][mUD.y] = MONSTERUD;
			loadMap(outline);
		}while(outline[mUD.x-1][mUD.y]!=WALL){
			sleep(2);
			outline[mUD.x][mUD.y] = FLOOR;
			if(outline[mUD.x-1][mUD.y]==CHARACTER){
				//game over situation
				printf("Sorry you couldn't finish the map.\n Monster Ate You !!!");
				exit(1);
			}
			outline[mUD.x-1][mUD.y] = MONSTERUD;
			loadMap(outline);

		}
	}
	return NULL;
}
void *controlMonster(){
	while(1){
		if(wakeMonsterUp()==1) {
			pthread_create(&sleepy_monster_thread,NULL,wake_up_monster,NULL);
			pthread_join(sleepy_monster_thread,NULL);
			pthread_cancel(control);
			break;
		}
	}
	return NULL;
}
void *wake_up_monster(){
	while(1){
			usleep(500000);
			int direction = rand()%2; //x= 0, y= 1
			int distance = sqrt(pow(c.x-mS.x,2) + pow(c.y-mS.y,2));
			if(distance<=1){
				printf("You are killed!!! (by Witch)\n\n");
				exit(1);
			}


			if(c.x< mS.x){
				if(outline[mS.x-1][mS.y]!=WALL){
					outline[mS.x][mS.y] = FLOOR;
					outline[mS.x-1][mS.y] = MONSTERSLEEPY;
				}
			}else if(c.x>mS.x){
					if(outline[mS.x+1][mS.y]!=WALL){
						outline[mS.x][mS.y] = FLOOR;
						outline[mS.x+1][mS.y] = MONSTERSLEEPY;
				}
			}
      loadMap(outline);
      drawMap();
			usleep(500000);
			if(c.y< mS.y){
				if(outline[mS.x][mS.y-1]!=WALL){
					outline[mS.x][mS.y] = FLOOR;
					outline[mS.x][mS.y-1] = MONSTERSLEEPY;
				}
			}else if(c.y>mS.y){
					if(outline[mS.x][mS.y+1]!=WALL){
						outline[mS.x][mS.y] = FLOOR;
						outline[mS.x][mS.y+1] = MONSTERSLEEPY;
				}
			}
      loadMap(outline);
      drawMap();





	}
	return NULL;
}

int wakeMonsterUp(){
	//control if character is enough close to monster or not
	int distance = sqrt(pow(c.x-mS.x,2) + pow(c.y-mS.y,2));
	if(distance<9) return 1;

	return -1;
}
