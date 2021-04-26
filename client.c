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


#include <stdio.h>
#include <curses.h> 		/* Primitives de gestion d'ecran */
#include <sys/signal.h>
#include <sys/wait.h>
#include<stdlib.h>
#include <unistd.h>
#include <string.h>


#include "fon.h"   		/* primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"
#define SERVEUR_DEFAUT "127.0.0.1"

#define COL_RED "\x1b[38;2;255;32;32m"
#define COL_GREEN "\x1b[38;2;32;255;32m"
#define COL_BLUE "\x1b[38;2;32;64;255m"
#define COL_YELLOW "\x1b[38;2;255;255;32m"
#define COL_PINK "\x1b[38;2;255;128;128m"
#define COL_PURPLE "\x1b[38;2;192;32;255m"
#define COL_ORANGE "\x1b[38;2;255;128;32m"
#define COL_WHITE "\x1b[38;2;255;255;255m"
#define COL_RESET "\e[0m"

#define COL_BLACK_W 0
#define COL_RED_W 1
#define COL_GREEN_W 2
#define COL_BLUE_W 3
#define COL_YELLOW_W 4
#define COL_PINK_W 5
#define COL_PURPLE_W 6
#define COL_ORANGE_W 7
#define COL_WHITE_W 8

char* CODE_STATES[] = {"⭘", "⬤"};
char* CODE_COLORS[] = {COL_RED, COL_GREEN, COL_BLUE, COL_YELLOW, COL_PURPLE, COL_ORANGE, COL_PINK, COL_WHITE};
int CODE_COLORS_W[] = {COL_RED_W, COL_GREEN_W, COL_BLUE_W, COL_YELLOW_W, COL_PURPLE_W, COL_ORANGE_W, COL_PINK_W, COL_WHITE_W};

char* HINT_STATES[] = {"•", "⭗", "⭗"};
char* HINT_COLORS[] = {COL_WHITE, COL_WHITE, COL_RED};


typedef struct
{
	char* name;
	int length;
	int nb_colors;
	int history_length;
}variation;

variation variations[] = {
	{"Original", 4, 6, 12},
	{"Deluxe", 8, 5, 14},
	{"Kids", 5, 3, 10},
	{"Binary", 4, 2, 5},
	{"Extreme", 8, 8, 18}
};


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
	
	tmp(serveur, service, &variations[0]);
	//client_appli(serveur, service);

	/* serveur est le nom (ou l'adresse IP) auquel le client va acceder */
	/* service le numero de port sur le serveur correspondant au  */
	/* service desire par le client */
	
	
	//client_appli(serveur,service);
}


void print_code(char* state, char* color, int size){
	char c=0;
	char s=0;
	for(int i=0; i < size; i++){
		c = (color[i/2] >> 4*(i%2)) & 0xF;
		s = (state[i/8] >> (i%8)) & 1;
		printf("%s", CODE_COLORS[c]);
		printf(" %s ", CODE_STATES[s]);
		printf(COL_RESET);
	}
}
void write_code(char* state, char* color, int size, WINDOW* w){
	char c=0;
	char s=0;
	int str_len = size*32;
	char* str = malloc(str_len);
	for(int i=0; i < size; i++){
		c = (color[i/2] >> 4*(i%2)) & 0xF;
		s = (state[i/8] >> (i%8)) & 1;
		
		wattron(w, COLOR_PAIR(CODE_COLORS_W[c]));
		sprintf(str, " %s ", CODE_STATES[s]);
		waddstr(w, str);
		wattroff(w, COLOR_PAIR(CODE_COLORS_W[c]));
		wrefresh(w);
	}
	//waddstr(w, str);
}
void print_hint(char* state, int size){
	char s=0;
	for(int i=0; i < size; i++){
		s = (state[i/4] >> 2*(i%4)) & 3;
		printf("%s", HINT_COLORS[s]);
		printf(" %s ", HINT_STATES[s]);
		printf(COL_RESET);
	}
}
void write_hint(char* state, int size, WINDOW* w){
	char s=0;
	int str_len = size*32;
	char* str = malloc(str_len);
	for(int i=0; i < size; i++){
		s = (state[i/4] >> 2*(i%4)) & 3;
		sprintf(str, "%s", HINT_COLORS[s]);
		sprintf(str, " %s ", HINT_STATES[s]);
		sprintf(str, COL_RESET);
	}
	waddstr(w, str);
}

void print_line(char* code_state, char* code_color, char* hint_state, int size){
	print_code(code_state, code_color, size);
	printf(" ");
	print_hint(hint_state, size);
	printf("\n");
}

void write_line(char* code_state, char* code_color, char* hint_state, int size, WINDOW* w){
	write_code(code_state, code_color, size, w);
	waddch(w, ' ');
	write_hint(hint_state, size, w);
	waddch(w, '\n');
}

void gen_hint(char* code, char* hidden, char* hint_state, int size){
	char cc=0;
	char ch=0;
	int* count_c = malloc(size*sizeof(int));
	int* count_h = malloc(size*sizeof(int));

	int hint_idx = 0;
	for(int i=0; i < size/4;i++){
		hint_state[i] = 0;
	}
	for(int i=0; i < size; i++){
		cc = (code[i/2] >> 4*(i%2)) & 0xF;
		ch = (hidden[i/2] >> 4*(i%2)) & 0xF;
		
		if(cc == ch){
			hint_state[hint_idx/4] |= (2 << 2*(hint_idx%4));
			hint_idx++;
		}
		else{
			count_c[cc]++;
			count_h[ch]++;
		}

	}

	for(int i=0; i < size; i++){
		int min = (count_h[i] > count_c[i]) ? count_c[i] : count_h[i];
		while(min){
			hint_state[hint_idx/4] |= (1 << 2*(hint_idx%4));
			min--;
			hint_idx++;
		}
	}
}
/*****************************************************************************/
void tmp (char *serveur,char *service, variation* var)
{	
	int code_length = var->length;
	int history_length = var->history_length;

	char hidden[] = {0 | (1 << 4), 0 | (0 << 4), 4 | (6 << 4), 5 | (7 << 4)};
	char code[] = {2 | (1 << 4), 0 | (3 << 4), 3 | (5 << 4), 6 | (7 << 4)};
	char code_state[] = {255};
	char hint_state[2];

	char** code_history = malloc(history_length*code_length/2);
	char** hint_history = malloc(history_length*code_length/4);

	print_code(code_state, hidden, 8);
	printf("\n");
	gen_hint(code, hidden, hint_state, 8);
	print_line(code_state, code, hint_state, 8);
	printf("\n");
	printf("%d\n", COLORS);
	
	WINDOW *W1, *L1, *C;
  	pid_t PID;
 	int SZ_1, SZ_2 ; /* taille d'un cadre */
 	char CAR ;
	
 	initscr(); /* Initialisation (nettoyage) de l'ecran */
	start_color();

	init_color(COL_BLACK_W, 125, 125, 125);
	init_color(COL_RED_W, 1000, 125, 125);
	init_color(COL_GREEN_W, 125, 1000, 125);
	init_color(COL_BLUE_W, 200, 125, 1000);
	init_color(COL_YELLOW_W, 1000, 1000, 125);
	init_color(COL_PINK_W, 1000, 500, 500);
	init_color(COL_PURPLE_W, 750, 125, 1000);
	init_color(COL_ORANGE_W, 1000, 500, 125);
    init_color(COL_WHITE_W, 1000, 1000, 1000);

	init_pair(COL_BLACK_W, COL_WHITE_W, COL_BLACK_W);
	init_pair(COL_RED_W, COL_RED_W, COL_BLACK_W);
	init_pair(COL_GREEN_W, COL_GREEN_W, COL_BLACK_W);
	init_pair(COL_BLUE_W, COL_BLUE_W, COL_BLACK_W);
	init_pair(COL_YELLOW_W, COL_YELLOW_W, COL_BLACK_W);
	init_pair(COL_PINK_W, COL_PINK_W, COL_BLACK_W);
	init_pair(COL_PURPLE_W, COL_PURPLE_W, COL_BLACK_W);
	init_pair(COL_ORANGE_W, COL_ORANGE_W, COL_BLACK_W);
	init_pair(COL_WHITE_W, COL_WHITE_W, COL_BLACK_W);
	
	/* printw("%i LINES %i COLS \n ", LINES, COLS);*/
	refresh();
	
	SZ_1 = LINES-5;
	SZ_2 = 4;
	if(SZ_1<13 || COLS < 38)
	{  endwin(); printf("ERREUR : Fenetre trop petite \n"); exit(1) ;}
	/* creation de trois cadres superposes separes par une ligne */
 	W1= newwin( SZ_1, COLS, 0, 0); 
	L1= newwin( 1, COLS, SZ_1, 0);
 	C= newwin(SZ_2, COLS,SZ_1+1, 0);
	/* pas de curseur */
 	leaveok(C,TRUE);
 	leaveok(L1,TRUE);
	leaveok(W1,TRUE);
	
	/* scroll automatique sur W1, W2, C */
	scrollok(W1,TRUE);scrollok(C,TRUE);
	/* tracer les lignes */
 	whline(L1, '=', COLS);
	wrefresh(L1);
	/* ecriture dans le cadre de controle */
 	wprintw(C,"  vous pouvez taper\n le . termine  ....\n");
 	wrefresh(C);
	
	/* recuperation des caracteres tels que frappes */
 	cbreak();
	/* pas d'echo automatique */
	noecho();
	
	while((CAR=getch())!='.')
	{	

		write_code(code_state, code, code_length, W1);
		waddstr(W1, "\n");
		wrefresh(W1);
	}
	
	wprintw(C,"on termine \n ....\n");
 	wrefresh(C);
	endwin();
}

//renvoie 1 si le joueur a gagné, 0 sinon
int tour(int serveur, int difficulte){
	char* buffer = malloc(sizeof(char) * 2);
	printf("votre choix %d de couleurs \n", difficulte);
	//ici on doit write dans le serveur les choix
	int nbRead = read(serveur, buffer, 2);
	int nbJustes = buffer[0]>>4;
	int nbCouleurs = buffer[0] & 0x0F;

	printf("il y a %d couleurs de justes \n", nbCouleurs);
	printf("il y a %d pionts de bien placés \n", nbJustes);

	if(nbJustes==difficulte)
		return 1;
	return 0;
}

void jeu(int serveur){

	int nbRead = 0;
	int difficulte = -1;
	char* buffer = malloc(sizeof(char) * 2);
	while(difficulte < 1){
		printf("quelle difficultée pour votre partie ? \n");

		nbRead = read(1, buffer, 2);
		difficulte = atoi(buffer);
	}

	//on envoie la difficultée choisie au serveur
	write(serveur, buffer, nbRead);

	int nbTour = 1;
	int gagne = 0;
	while( !gagne && nbTour !=12){
		gagne = tour(serveur, difficulte);
		nbTour++;
	}

	if(gagne)
		printf("bien joué vous avez gagné en %d coups \n", nbTour);
	else
		printf("désolé vous avez perdu \n");
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
 }

/*****************************************************************************/

