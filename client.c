/******************************************************************************/
/*			Application: ...					*/
/******************************************************************************/
/*									      */
/*			 programme  CLIENT				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs : ... 					*/
/*									      */
/******************************************************************************/	

#define _XOPEN_SOURCE_EXTENDED
#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include "locale.h"
#include "time.h"
#include "fon.h"   		/* primitives de la boite a outils */
#include "shared.h"

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"



void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/
void tmp(char* s, char* i, variation* v);
int main(int argc, char *argv[])
{
	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

	//tmp(serveur, service);
	//client_appli(serveur,service);
	/* Permet de passer un nombre de parametre variable a l'executable */
	switch(argc)
	{
 	case 1 :		/* arguments par defaut */
		  printf("serveur par defaut: %s\n",serveur);
		  printf("service par defaut: %s\n",service);
		  break;
  	case 2 :		/* serveur renseigne  */
		  serveur=argv[1];
		  printf("service par defaut: %s\n",service);
		  break;
  	case 3 :		/* serveur, service renseignes */
		  serveur=argv[1];
		  service=argv[2];
		  break;
    default:
		  printf("Usage:client serveur(nom ou @IP)  service (nom ou port) \n");
		  exit(1);
	}
	
	//tmp(serveur, service, &variations[0]);
	client_appli(serveur, service);

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	
	//client_appli(serveur,service);
}



void write_code(char* state, char* color, int size, WINDOW* w){
	char c=0;
	char s=0;
	int str_len = size*32;
	wchar_t* str = malloc(str_len*sizeof(wchar_t));
	for(int i=0; i < size; i++){
		c = (color[i/2] >> 4*(i%2)) & 0xF;
		s = (state[i/8] >> (i%8)) & 1;
		
		wattron(w, COLOR_PAIR(CODE_COLORS_W[c]));
		swprintf(str, str_len, L" %ls ", CODE_STATES[s]);
		waddwstr(w, str);
		wattroff(w, COLOR_PAIR(CODE_COLORS_W[c]));
	}
	free(str);
}
void write_hint(char* state, int size, WINDOW* w){
	char s=0;
	int str_len = size*32;
	wchar_t* str = malloc(str_len*sizeof(wchar_t));
	for(int i=0; i < size; i++){
		s = (state[i/4] >> 2*(i%4)) & 3;
		wattron(w, COLOR_PAIR(HINT_COLORS_W[s]));
		swprintf(str, str_len, L" %ls ", HINT_STATES[s]);
		waddwstr(w, str);
		wattroff(w, COLOR_PAIR(HINT_COLORS_W[s]));
	}
	free(str);

}


void write_line(char* code_state, char* code_color, char* hint_state, int size, WINDOW* w){
	write_code(code_state, code_color, size, w);
	waddch(w, ' ');
	write_hint(hint_state, size, w);
	waddch(w, '\n');
}

void init_gui(){
	initscr(); /* Initialisation (nettoyage) de l'ecran */
	start_color();

	init_color(COL_BLACK_W, 125, 125, 125);
	init_color(COL_RED_W, 1000, 125, 125);
	init_color(COL_GREEN_W, 125, 1000, 125);
	init_color(COL_BLUE_W, 200, 125, 1000);
	init_color(COL_YELLOW_W, 1000, 1000, 125);
	init_color(COL_PINK_W, 1000, 500, 500);
	init_color(COL_PURPLE_W, 750, 125, 1000);
    init_color(COL_WHITE_W, 1000, 1000, 1000);
	init_color(COL_ORANGE_W, 1000, 500, 125);

	init_pair(COL_BLACK_W, COL_WHITE_W, COL_BLACK_W);
	init_pair(COL_RED_W, COL_RED_W, COL_BLACK_W);
	init_pair(COL_GREEN_W, COL_GREEN_W, COL_BLACK_W);
	init_pair(COL_BLUE_W, COL_BLUE_W, COL_BLACK_W);
	init_pair(COL_YELLOW_W, COL_YELLOW_W, COL_BLACK_W);
	init_pair(COL_PINK_W, COL_PINK_W, COL_BLACK_W);
	init_pair(COL_PURPLE_W, COL_PURPLE_W, COL_BLACK_W);
	init_pair(COL_ORANGE_W, COL_ORANGE_W, COL_BLACK_W);
	init_pair(COL_WHITE_W, COL_WHITE_W, COL_BLACK_W);
}

void init_windows(WINDOW* W1, WINDOW* L1, WINDOW* C1){
	/* pas de curseur */
 	leaveok(C1,TRUE);
 	leaveok(L1,TRUE);
	leaveok(W1,TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);

	/* scroll automatique sur W1, W2, C */
	scrollok(W1,TRUE);scrollok(C1,TRUE);
	refresh();
	/* tracer les lignes */
	 /* recuperation des caracteres tels que frappes */
 	cbreak();
	/* pas d'echo automatique */
	noecho();
	
	setlocale(LC_ALL, "");
}

void draw_bottom_windows(WINDOW* L1, WINDOW* C1){
	whline(L1, '_', COLS);
	wrefresh(L1);
	refresh();

 	wrefresh(C1);
	refresh();
}
void draw_menu(WINDOW* W, int selected){
	char str[48];
	int size = sizeof(variations)/sizeof(variation);
	printf("V %d\n", size);
	for(int i=0; i < size; i++){
		if(i == selected)
			sprintf(str, " > %s\n", variations[i].name);
		else
			sprintf(str, "   %s\n", variations[i].name);
		
		waddstr(W, str);
	}
	sprintf(str, 
			"\n\nINFO: %d trous, %d couleurs, %d essais\n", 
			variations[selected].length, 
			variations[selected].nb_colors, 
			variations[selected].history_length
		);
	
	waddstr(W, str);
}

int menu(WINDOW* W){
	int selected = 0;
	int menu_size = sizeof(variations)/sizeof(variation);
	int CAR = 0;
	
	do
	{	
		int move = 0;

		wclear(W);
		waddstr(W, "Select rules: \n\n");
		
		if(CAR == KEY_UP) move = -1;
		if(CAR == KEY_DOWN) move = 1;

		selected = (menu_size + selected + move) % menu_size;
		draw_menu(W, selected);
		//write_code(code_state, code, code_length, W1);
		
		wrefresh(W);
	}while((CAR=getch())!=10);

	return selected;
}


void init_tour(variation* var, char** states, char** colors, char** fstates, char** palette){
	int s = (var->length+7)/8;
	*states = malloc(s);
	for(int i=0; i<s;i++) (*states)[i] = 0;

	int c = (var->length+1/2);
	*colors = malloc(c);
	for(int i=0; i<var->length;i++){
		(*colors)[i/2] &= ~(0xF << 4*(i%2));
		(*colors)[i/2] |= COL_WHITE_W << 4*(i%2);
	}


	int fs = (var->length+7)/8;
	*fstates = malloc(fs);
	for(int i=0; i<fs;i++) (*fstates)[i] = 0xFF;

	int p = (var->nb_colors+1/2);
	
	(*palette) = malloc(p);
	for(int i=0; i<var->nb_colors;i++){
		(*palette)[i/2] &= ~(0xF << 4*(i%2));
		(*palette)[i/2] |= i << 4*(i%2);
	}
}

int select_color(variation* var, WINDOW* C, char* fstates, char* palette, int selected){
	int CAR;
	do{
		int move = 0;
		wclear(C);
		waddstr(C, "Select colors: \n\n");
		write_code(fstates, palette, var->nb_colors, C);
		waddwstr(C, L"\n");

		if(CAR == KEY_LEFT) move = -1;
		if(CAR == KEY_RIGHT) move = 1;
		
		selected = (var->nb_colors + selected + move) % var->nb_colors;
		//waddch(C, ' ');
		for(int j=0; j < var->nb_colors; j++){
			if(j == selected)
				waddwstr(C, L" ⸺ ");
			else
				waddwstr(C, L"   ");

		}
		wrefresh(C);
		refresh();
	}while( (CAR = getch()) != 10);
	return selected;
}
//renvoie 1 si le joueur a gagné, 0 sinon
int tour(int serveur, variation* var, WINDOW* W, WINDOW* C){
	
	char *states, *colors, *fstates, *palette;
	init_tour(var, &states, &colors, &fstates, &palette);

	int idx=0;
	int selected = 0;

	waddstr(W, "\n");
	write_code(states, colors, var->length, W);
	wrefresh(W);
	do{
		selected = select_color(var, C, fstates, palette, selected);

		colors[idx/2] &= ~(0xF << 4*(idx%2));
		colors[idx/2] |= (selected << 4*(idx%2));
		states[idx/8] |= (1 << (idx%8));
		
		waddstr(W, "\r");
		write_code(states, colors, var->length, W);
		wrefresh(W);

	}while(++idx < var->length);

	char* hint_state = malloc((var->length+3)/4);

	int g = 0;

	h_writes(serveur, colors, (var->length+1)/2);
	
	h_reads(serveur, hint_state, (var->length+3)/4);
	h_reads(serveur, (char*)&g, sizeof(int));
	
	write_hint(hint_state, var->length, W);
	wrefresh(W);

	free(hint_state);
	free(states);
	free(colors);
	free(fstates);
	free(palette);

	return g;
}

void fail_screen(int serveur, WINDOW* W, variation* var){
	waddstr(W, "\n\n\nFailed to break the code\n");

	char* hidden = malloc((var->length+1)/2);
	h_reads(serveur, hidden, (var->length+1)/2);
	
	int fs = (var->length+7)/8;
	char* fstates = malloc(fs);
	for(int i=0; i<fs;i++) fstates[i] = 0xFF;
	write_code(fstates, hidden, var->length, W);

	free(hidden);
	free(fstates);
}

int loop(int serveur, variation* var, WINDOW* W, WINDOW* C){

	wclear(W);

	mvwhline(W, var->history_length+1, 1, '-', var->length*3);
	wmove(W, 0, 0);
	wrefresh(W);
	int nbTour = 0;
	int gagne = 0;
	while( !gagne && nbTour < var->history_length){
		gagne = tour(serveur, var, W, C);
		nbTour++;
	}


	wclear(W);
	wclear(C);
	if(gagne){
		char str[32];
		sprintf(str, "\n\n\nCode broken in %d attempts\n\n", nbTour);
		waddstr(W, str);
	}
	else{
		fail_screen(serveur, W, var);
	}
	
	
	waddstr(W, "\n\n continue...");
	wrefresh(W);
	wrefresh(C);
	refresh();
	
	getch();

}
void jeu(int serveur){

	init_gui();
	
	int SZ_1 = LINES-5;
	int SZ_2 = 4;

	/* création de 2 cadres sépares par une ligne */
	WINDOW *W1 = newwin( SZ_1, COLS, 0, 0); 
	WINDOW *L1 = newwin( 1, COLS, SZ_1, 0);
	WINDOW *C1 = newwin(SZ_2, COLS,SZ_1+1, 0);
 	
	if(SZ_1<20 || COLS < 38)
		{endwin(); printf("ERREUR : Fenetre trop petite \n"); exit(1) ;}
	

	init_windows(W1, L1, C1);
	
	int selected = menu(W1);
	write(serveur, &selected, sizeof(int));
	
	variation var = variations[selected];
	
	draw_bottom_windows(L1, C1);
	loop(serveur, &var, W1, C1);
	wclear(W1);
	wclear(C1);
	wclear(L1);
	use_default_colors();
	
	nocbreak();
	echo();
	refresh();
	endwin();
} 

/*****************************************************************************/
void client_appli (char *serveur,char *service){

	int socket = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in* adrCli = malloc(sizeof(struct sockaddr_in));
	struct sockaddr_in* adrServ = malloc(sizeof(struct sockaddr_in));
	adr_socket( "0" , NULL , SOCK_STREAM, &adrCli);
	adr_socket( service , serveur , SOCK_STREAM, &adrServ);
	h_bind(socket, adrCli);
	h_connect(socket, adrServ);
	jeu(socket);
	h_close(socket);
	//free(adrCli);
	//free(adrServ);
 }

/*****************************************************************************/

