#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<netinet/in.h>
#include<time.h>
#include<math.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define HEAD '0'
#define BODY '#'
#define FRUIT '&'
#define SUPERFRUIT '$'
#define SERVADDR "127.0.0.1"
#define SERVPORT 1345

WINDOW *create_new_win(int height, int width, int starty, int startx){
	WINDOW *local_win;
	local_win = newwin(height,width,starty,startx);
	box(local_win, 0, 0);
	wrefresh(local_win);
	return local_win;
}

    
void singleplay(int row, int col, char* name){
	FILE *scoreboard;
	WINDOW *game_win;
        struct sockaddr_in servaddr;
	int body_loc[1000][2] = {0};
	int length;
	int pos[2] = {col/2,(row-20)/2};
	int xdif,ydif, sockfd,n;
	char lose = 0;
	short ch;
	char direction = 0;
	char frup = 1;
	int fruitloc[2],supfruitloc[2];
	int score = 0;
	long life = 100;
	int supfruittimer = 0;
	char supfrup = 0;
	char first[10];
	int first_score;
	char scorer[10];
	char loc = 4;
        char * reader = "Please Read";
        char * writer = "Please Write";
	halfdelay(1); //sets the screen refresh rate .1s
	keypad(stdscr, TRUE);	
	WINDOW *score_win; 
	game_win = create_new_win(col, row-20,0,0); //creates the game board
	score_win = create_new_win(col,19,0,row-20); //creates the score board
	wrefresh(game_win);
	wrefresh(score_win);
	length = 3;
	fruitloc[0] = rand()%(col-1);
	fruitloc[1] = rand()%(row-21);
	//Scoreboard
        /* LOCAL SCOREBOARD
	scoreboard = fopen("scores.board","r");
	while(fgets(first,100,scoreboard)!=NULL){
		first[strcspn(first,"\n")] = 0;
		if(fgets(scorer,10,scoreboard)!=NULL){
			first_score = atoi(scorer);
			mvwprintw(score_win,loc,1, "%-8s:%5d",first,first_score);

			loc++;
		}
	}
	fclose(scoreboard);
	fgets(first, 10, scoreboard);
	fgets(scorer,10, scoreboard);
	first_score = atoi(scorer);
	fgets(second, 10, scoreboard);
	fgets(scorer,10,scoreboard);
	second_score = atoi(scorer);
	fgets(third, 10, scoreboard);
	fgets(scorer,10,scoreboard);
	third_score = atoi(scorer);
	*/

        /*Global Scoreboard */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(SERVADDR);
        servaddr.sin_port = htons(SERVPORT);
        connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        n = write(sockfd, reader, strlen(reader));
        printf("Please Read");
        while(strcmp(first,"That's All")!=0){
            read(sockfd, first, 10);
            first[strcspn(first, "\n")] = 0;
            if(read(sockfd, scorer, 10)!=0){
                first_score = atoi(scorer);
                mvwprintw(score_win, loc, 1, "%-8s:%5d",first, first_score);
                loc++;
            }
        }
        close(sockfd);
        /* END SCOREBOARD READING */

	while(!(lose)) {
		wrefresh(game_win);  //Refresh the screen
		mvwprintw(score_win,1,1,"Health: % 8d",life);
		mvwprintw(score_win,2,1,"Score: % 8d",score);
		mvwprintw(score_win,3,1,"High Scores");
		//mvwprintw(score_win,4,1,"%s %d", first, first_score);
		//mvwprintw(score_win,5,1,"%s %d", second, second_score);
		//mvwprintw(score_win,6,1,"%s %d", third, third_score);
		life--;
		//score++;
		if(life<0){
			lose=1;
		}
		wrefresh(score_win);
		werase(game_win);
		box(game_win,0,0);
		if((ch = getch())!= ERR){ // Read user direction
			switch(ch){
				case KEY_LEFT:
					if(direction!=1){
					direction = 3;
					}
					break;
				case KEY_DOWN:
					if(direction!=0){
					direction = 2;
					}
					break;
				case KEY_RIGHT:
					if(direction!=3){
					direction = 1;
					}
					break;
				case KEY_UP:
					if(direction!=2){
					direction = 0;
					}
					break;
			}
		}
		switch(direction){ //Move
			case 3:
				pos[1] = pos[1] - 1;
				break;
			case 2:
				pos[0] = pos[0] + 1;
				break;
			case 1:
				pos[1] = pos[1] + 1;
				break;
			case 0:
				pos[0] = pos[0] - 1;
				break;
		}
		
		body_loc[0][0] = pos[0]; //Move head
		body_loc[0][1] = pos[1];
		if((pos[0] == 0)|(pos[0] == col-1)|(pos[1] == 0)|(pos[1] == (row - 21))){ //Check if hit wall
			lose = 1; //Check if hit wall
		}
		for(int i = length; i > 0; i--){ 
			if((body_loc[i][0] == pos[0])&&(body_loc[i][1] == pos[1])){ // Check if hit self
				lose = 1;
				break;
			}
				//wprintw(game_win,"%d", length);
				body_loc[i][0] = body_loc[i-1][0];
				body_loc[i][1] = body_loc[i-1][1];
				mvwprintw(game_win,body_loc[i+1][0],body_loc[i+1][1], "%c",BODY);
				//wprintw(game_win,"%d",i);
				//getch();
			
		}
		
		if(!(frup)){ //if fruit isn't present aka has been eaten
	
			body_loc[length][0] = body_loc[length-1][0]; //Increase the length by one
			body_loc[length][1] = body_loc[length-1][1]; //due to the eaten fruit
			fruitloc[0] = (rand()%(col-4))+2; //randomly select a new location for fruit
			fruitloc[1] = (rand()%(row-24))+2;
			xdif = abs(fruitloc[0]-pos[0]);
			ydif = abs(fruitloc[1]-pos[1]);
			life = life + 3*(xdif + ydif);
			frup = 1; //fruit on board set to true
		}
		if((fruitloc[0] == pos[0])&&(fruitloc[1]== pos[1])){ //if fruit eaten
			frup = 0; //set fruit on board to false
			length += 1; //incease length
			//life +=  40+length; //increase health
			score += 10; //increase score
		}
		if((score%25==0)&&(supfrup==0)&&(score > 0)){
			supfruitloc[0] = (rand()%(col-3))+1; //randomly select a new location for fruit
			supfruitloc[1] = (rand()%(row-23))+1;
			supfruittimer = 25;
			supfrup = 1; 
		}
		if((supfruitloc[0] == pos[0])&&(supfruitloc[1]== pos[1])){ //if super fruit eaten
			supfruitloc[1] = 0;
			supfruitloc[0] = 0;
			supfrup = 0; //set super fruit on board to false
			length += 1; //incease length
			life +=  100+length; //increase health
			score += 60; //increase score
		}
		if((supfruittimer==0)&&(supfrup==1)){
			supfruitloc[0] = (rand()%(col-4))+2; //randomly select a new location for  super fruit
			supfruitloc[1] = (rand()%(row-24))+2;
			supfruittimer = 25; //reset the super fruit move timer
		}
		if(supfruittimer>0){
			supfruittimer--; // reduce the superfruit move timer
		}
		if(supfrup){
			mvwprintw(game_win,supfruitloc[0],supfruitloc[1],"%c",SUPERFRUIT); //print superfruit
		}
		mvwprintw(game_win,fruitloc[0],fruitloc[1], "%c", FRUIT); //print fruit
		mvwprintw(game_win,pos[0], pos[1], "%c", HEAD);		
	}


        /* START SCOREBOARD WRITE */
        sockfd = socket(AF_INET, SOCK_STREAM,0);
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(SERVADDR);
        servaddr.sin_port = htons(SERVPORT);
        connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        n = write(sockfd, writer, strlen(reader));
        sprintf(name, "%8s", name);
        n = write(sockfd, name, strlen(name));
        sprintf(scorer, "%d",score);
        n = write(sockfd, scorer, strlen(scorer));
        close(sockfd);
        /*END SCOREBOARD READ */

	wrefresh(game_win);
	werase(game_win);
	mvwprintw(game_win, (col)/2, (row-strlen(name)-30)/2-20 , "Congrats %s you got a score of % 4d",name, score);
	mvwprintw(game_win, (col)/2 + 1, (row- 31)/2-20, "Press c to continue.");
	wrefresh(game_win);

	while(( supfrup = getch()) != 'c'){
	       wrefresh(game_win);
	}
	werase(score_win);
	wrefresh(score_win);
	return;

}

WINDOW * main_options(WINDOW * screen, int row, int col, char name[100]){
	char game_choice[] = "What game would you like to play?";
	char single[] = "1. Singleplayer";
	char multi[] = "2. Multiplayer";	
	mvwprintw(screen,24/2,(80-strlen(name)-8)/2 , "Welcome %s",name);
	mvwprintw(screen,24/2+1,(80-strlen(game_choice))/2, "%s",game_choice); 
	mvwprintw(screen,24/2+2,(80-strlen(single))/2,"%s",single);
//	mvwprintw(screen,24/2+3,(80-strlen(single))/2,"%s",multi);
	mvwprintw(screen,24/2+3,(80-strlen(single))/2,"3. Quit");
	return screen;
}
int main(){
	char name_grab[] = "What is your name? ";
	char name[100];
	char input;
	WINDOW *main_screen;
	int row,col;
	noecho();
	initscr();
	if((LINES != 24) & (COLS != 80)){
		printf("Please set screen to 80 x 24. [W x H]\n");
		return 1;
	}
	row = LINES;
	col = COLS;
	cbreak();
	curs_set(0);
	refresh();
	main_screen = create_new_win(24,80,0,0);
	//wprintw("%s", name_grab);
	mvwprintw(main_screen, 24/2,(80-strlen(name_grab))/2, "%s", name_grab);
	wrefresh(main_screen);

	wgetstr(main_screen, name);
	werase(main_screen);
	box(main_screen,0,0);
	main_screen = main_options(main_screen, row, col, name);	
	wrefresh(main_screen);
	
	while((input = getch())){
		switch(input){
			case '1':
				werase(main_screen);
				delwin(main_screen);
				singleplay(80,24,name);
				break;
				werase(main_screen);
			case '3':
				endwin();
				return 0;
			default:	
				werase(main_screen);
				mvwprintw(main_screen,24/2-1,(80-18)/2 , "That is not an option.");
				main_screen = main_options(main_screen, row, col, name);
				wrefresh(main_screen);
				break;
		}
	}
}

