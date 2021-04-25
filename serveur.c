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

#include "fon.h"     		/* Primitives de la boite a outils */

#define SERVICE_DEFAUT "1111"

void serveur_appli (char *service);   /* programme serveur */


/******************************************************************************/	
/*---------------- programme serveur ------------------------------*/

int main(int argc,char *argv[])
{

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

/*
int nbCouleursJustes(int* couleurs, char* choix, int difficulte){
	int nbJuste = 0;
	for(int i=0; i<difficulte; i++){
		int couleurCourante = 0;
		if(i%2==0)	
			couleurCourante = atoi(choix[i/2] && 0x0F);
		else
			couleurCourante = atoi(choix[i/2]  >> 4);
		if(tab[i]==couleurCourante)
			nbJuste++;
	}
	return nbJuste;
}

int* tirerCouleurs(int difficulte){
	int* tab = malloc(sizeof(int) * difficulte);
	for(int i=0; i<difficulte; i++){
		tab[i]=rand()%difficulte;
	}
	return tab;
}

//renvoie 1 si le joueur a gagné, 0 sinon
int tour(int socketClient, int* couleurs, int difficulte){
	
	char* buffer = malloc(sizeof(char)*(difficulte+1));
	//le client envoie son choix
	int nbRead = h_reads(socketClient, buffer, difficulte+1);

	int nbJustes = nbCouleursJustes(couleurs, buffer, difficulte);

	//on envoie le nombre de choix justes au client
	write(socketClient, atoi(nbJuste), 1);

	if(nbJustes==difficulte)
		return 1;
	return 0;
}


void jeu(int socketClient){

	char* buffer = malloc(sizeof(char)*256);

	//debut de communication, le client doit envoyer un int correspondant à la difficulté

	int nbRead = h_reads(socketClient, buffer, 256);
	int difficulte = atoi(buffer);

	if(difficulte<1)
		exit(1);

	int* couleurs = tirerCouleurs();

	int nbTour = 1;
	while(!tour()) || nbTour !=12)
		nbTour++;
	
	
	write(0, buffer, nbRead);
	exit(1);
}
*/


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
		char* buffer = malloc(sizeof(char)*256);
		int nbRead = h_reads(client, buffer, 256);
		write(0, buffer, nbRead);
		//jeu(client);
	}

	}
}

/******************************************************************************/	
