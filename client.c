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

char* CODE_STATES[] = {"⭘", "⬤"};
char* CODE_COLORS[] = {COL_RED, COL_GREEN, COL_BLUE, COL_YELLOW, COL_PURPLE, COL_ORANGE, COL_PINK, COL_WHITE};

char* HINT_STATES[] = {"•", "⭗", "⭗"};
char* HINT_COLORS[] = {COL_WHITE, COL_WHITE, COL_RED};

char** code_history;
char** states_history;

void client_appli (char *serveur, char *service);


/*****************************************************************************/
/*--------------- programme client -----------------------*/
void tmp(char* s, char* i);
int main(int argc, char *argv[])
{	
	
	char *serveur= SERVEUR_DEFAUT; /* serveur par defaut */
	char *service= SERVICE_DEFAUT; /* numero de service par defaut (no de port) */

	tmp(serveur, service);
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
	
	client_appli(serveur, service);

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

void print_hint(char* state, int size){
	char s=0;
	for(int i=0; i < size; i++){
		s = (state[i/4] >> 2*(i%4)) & 3;
		printf("%s", HINT_COLORS[s]);
		printf(" %s ", HINT_STATES[s]);
		printf(COL_RESET);
	}
}

void print_line(char* code_state, char* code_color, char* hint_state, int size){
	print_code(code_state, code_color, size);
	printf(" ");
	print_hint(hint_state, size);
	printf("\n");
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
void tmp (char *serveur,char *service)
{
	int code_length = 8;
	int history_length = 12;
	char hidden[] = {0 | (1 << 4), 0 | (0 << 4), 4 | (6 << 4), 5 | (7 << 4)};
	char code[] = {0 | (1 << 4), 2 | (3 << 4), 3 | (5 << 4), 6 | (7 << 4)};
	char code_state[] = {255};
	char hint_state[2];

	char** code_history = malloc(history_length*code_length/2);
	char** hint_history = malloc(history_length*code_length/4);

	print_code(code_state, hidden, 8);
	printf("\n");
	gen_hint(code, hidden, hint_state, 8);
	print_line(code_state, code, hint_state, 8);
	printf("\n");

	return;
	WINDOW *W1, *L1, *C;
  	pid_t PID;
 	int SZ_1, SZ_2 ; /* taille d'un cadre */
 	char CAR ;
	
 	initscr(); /* Initialisation (nettoyage) de l'ecran */
	/* printw("%i LINES %i COLS \n ", LINES, COLS);*/
	refresh();
	
	SZ_1 = LINES-5;
	SZ_2 = 4;
	if(SZ_1<6 || COLS < 12)
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
		waddch(W1,CAR);
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

