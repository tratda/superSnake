#include<stdlib.h>
#include<string.h>
#include<ncurses.h>
#include<netinet/in.h>
#include<time.h>

WINDOW *create_new_win(int height, int width, int starty, int startx){
	WINDOW *local_win;
	local_win = newwin(height,width,starty,startx);
	box(local_win, 0, 0);
	wrefresh(local_win);
	return local_win;
}


void singleplay(int row, int col){
	char body = '#';
	char head = '0';
	WINDOW *game_win;
	int body_loc[1000][2];
	int length;
	int pos[2] = {col/2,(row-20)/2};
	char lose = 0;
	short ch;
	char direction = 0;
	char frup = 1;
	char fruit = '&';
	int fruitloc[2];
	halfdelay(1);
	keypad(stdscr, TRUE);	
	WINDOW *score_win;
	game_win = create_new_win(col, row-20,0,0);
	score_win = create_new_win(col,19,0,row-20);
	wrefresh(game_win);
	wrefresh(score_win);
	length = 0;
	fruitloc[0] = rand()%(col-1);
	fruitloc[1] = rand()%(row-21);
	
	while(!(lose)){
		wrefresh(game_win);  //Refresh the screen
		werase(game_win);
		box(game_win,0,0);
		if((ch = getch())!= ERR){ // Read user direction
			switch(ch){
				case KEY_LEFT:
					direction = 3;
					break;
				case KEY_DOWN:
					direction = 2;
					break;
				case KEY_RIGHT:
					direction = 1;
					break;
				case KEY_UP:
					direction = 0;
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
		if((pos[0] == 0)|(pos[0] == col)|(pos[1] == 0)|(pos[1] == (row - 20))){
			lose = 1; //Check if hit wall
		}
		if(!(frup)){
	
			for(int i = 2; i <= length; i++){
				if((body_loc[i][0] == pos[0])&&(body_loc[i][1] == pos[1])){ // Check if hit self
					lose = 1;
					break;
				}
				body_loc[i][0] = body_loc[i][0];
				body_loc[i][1] = body_loc[i][1];
				
				mvwprintw(game_win,body_loc[i][0],body_loc[i][1], "%c",body);
			
			}
			body_loc[length][0] = body_loc[length-1][0];
			body_loc[length][1] = body_loc[length-1][1];			
			fruitloc[0] = rand()%(col-1);
			fruitloc[1] = rand()%(row-21);
			frup = 1;
		}
		else{
	
			for(int i = 1; i <= length; i++){
				if((body_loc[i][0] == pos[0])&&(body_loc[i][1] == pos[1])){ // Check if hit self
					lose = 1;
					break;
				}
				//wprintw(game_win,"%d", length);
				body_loc[i][0] = body_loc[i-1][0];
				body_loc[i][1] = body_loc[i-1][1];
				mvwprintw(game_win,body_loc[i+1][0],body_loc[i+1][1], "%c",body);
				//wprintw(game_win,"%d",i);
				//getch();
			
			}
		
		}
		if((fruitloc[0] == pos[0])&&(fruitloc[1]== pos[1])){
			frup = 0;
			length += 1;
		}
			//mvwprintw(game_win,0,0,"%d", length);
		
		mvwprintw(game_win,fruitloc[0],fruitloc[1], "%c", fruit);
		mvwprintw(game_win,pos[0], pos[1], "%c", head);
			
	}
	
}

int main(){
	char name_grab[] = "What is your name? ";
	char game_choice[] = "What game would you like to play?";
	char single[] = "1. Singleplayer";
	char multi[] = "2. Multiplayer";
	char name[100];
	char input;
	WINDOW *main_screen;
	int row,col;
	noecho();
	initscr();
	row = LINES;
	col = COLS;
	cbreak();
	curs_set(0);
	refresh();

	main_screen = create_new_win((LINES),(COLS),0,0);
	//wprintw("%s", name_grab);
	mvwprintw(main_screen, LINES/2,(COLS-strlen(name_grab))/2, "%s", name_grab);
	wrefresh(main_screen);

	wgetstr(main_screen, name);
	werase(main_screen);
	box(main_screen,0,0);	
	mvwprintw(main_screen,row/2,(col-strlen(name)-8)/2 , "Welcome %s",name);
	mvwprintw(main_screen,row/2+1,(col-strlen(game_choice))/2, "%s",game_choice); 
	mvwprintw(main_screen,row/2+2,(col-strlen(single))/2,"%s",single);
	mvwprintw(main_screen,row/2+3,(col-strlen(single))/2,"%s",multi);
	wrefresh(main_screen);
	
	input = getch();
	if(input=='1'){
		werase(main_screen);
		delwin(main_screen);
		singleplay(col,row);

	}
	endwin();
}
