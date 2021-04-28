/******************************************************************************/
/*			Application: ....			              */
/******************************************************************************/
/*									      */
/*			 programme  SERVEUR 				      */
/*									      */
/******************************************************************************/
/*									      */
/*		Auteurs :  ....						      */
/*		Date :  ....						      */
/*									      */
/******************************************************************************/	

#include<stdio.h>
#include <curses.h>

#include<sys/signal.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include "time.h"

#include "fon.h"     		/* Primitives de la boite a outils */
#include "shared.h"

#define SERVICE_DEFAUT "1111"

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{
	srand( time( NULL ) );
	char *service= SERVICE_DEFAUT; /* numero de service par defaut */


	/* Permet de passer un nombre de parametre variable a l'executable */
	switch (argc)
 	{
   	case 1:
		  printf("defaut service = %s\n", service);
		  		  break;
 	case 2:
		  service=argv[1];
            break;

   	default :
		  printf("Usage:serveur service (nom ou port) \n");
		  exit(1);
 	}

	/* service est le service (ou numero de port) auquel sera affecte
	ce serveur*/
	
	serveur_appli(service);
}


int gen_hint(char* code, char* hidden, char* hint_state, variation* var){
	char cc=0;
	char ch=0;
	int* count_c = malloc(var->nb_colors*sizeof(int));
	int* count_h = malloc(var->nb_colors*sizeof(int));

	int hint_idx = 0;
	for(int i=0; i < var->length; i++){
		cc = (code[i/2] >> 4*(i%2)) & 0xF;
		ch = (hidden[i/2] >> 4*(i%2)) & 0xF;
		
		if(cc == ch){
			hint_state[hint_idx/4] &= ~(3 << 2*(hint_idx%4));
			hint_state[hint_idx/4] |= (2 << 2*(hint_idx%4));
			hint_idx++;
		}
		else{
			count_c[cc]++;
			count_h[ch]++;
		}

	}

	if(hint_idx == var->length){
		free(count_c);
		free(count_h);
		return 1;
	}

	for(int i=0; i < var->nb_colors; i++){
		int min = (count_h[i] > count_c[i]) ? count_c[i] : count_h[i];
		while(min){
			hint_state[hint_idx/4] |= (1 << 2*(hint_idx%4));
			min--;
			hint_idx++;
		}
	}
	free(count_c);
	free(count_h);
	return 0;
}

void gen_hidden(variation* var, char* hidden){
	for(int i=0; i < var->length; i++){
		int r = rand() % var->nb_colors;
		hidden[i/2] &= ~(0xF << 4*(i%2));
		hidden[i/2] |= (r << 4*(i%2));
	}
}

//renvoie 1 si le joueur a gagné, 0 sinon
int tour(int socketClient, char* hidden, variation* var){

	int len = (var->length+1)/2;
	char* buffer = malloc(len);

	int len_hint = (var->length+3)/4;
	char* hint_state = malloc(len_hint);
	//le client envoie son choix
	int nbRead = h_reads(socketClient, buffer, len);


	int g = gen_hint(buffer, hidden, hint_state, var);

	//on envoie le nombre de choix justes au client
	write(socketClient, hint_state, len_hint);
	write(socketClient, &g, sizeof(int));

	//free(buffer);
	free(hint_state);
	return g;
}


void jeu(int socketClient){

	char* buffer = malloc(sizeof(int));

	//debut de communication, le client doit envoyer un int correspondant au mode de jeu

	int nbRead = h_reads(socketClient, buffer, sizeof(int));
	int mode = *((int*)buffer);
	int nbVar = sizeof(variations)/sizeof(variation);

	if(mode<0 || mode >= nbVar){
		free(buffer);
		return;
	}
	
	variation var = variations[mode];
	printf("mode de jeu : %s \n", var.name);

	printf("tirage des couleurs \n");

	char* hidden = malloc((var.length+1)/2);
	gen_hidden(&var, hidden);

	printf("\n debut des tours \n");

	int nbTour = 0;
	int gagne = 0;
	while( !gagne && nbTour < var.history_length){
		gagne = tour(socketClient, hidden, &var);
		nbTour++;
	}
	if(!gagne)
		h_writes(socketClient, hidden, (var.length+1)/2);

	free(buffer);
	free(hidden);
	
	
}



/******************************************************************************/	
void serveur_appli(char *service){

	int socket = h_socket(AF_INET, SOCK_STREAM);
	struct sockaddr_in* adr = malloc(sizeof(struct sockaddr_in));
	adr_socket( service , NULL, SOCK_STREAM, &adr);
	h_bind(socket, adr);
	h_listen(socket, 5);

	while(1){
		int client = h_accept(socket, adr);

		if(fork()==0){
			jeu(client);
			h_close(client);
			exit(1);
		}

	}
}

/******************************************************************************/	
