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


char* char_to_string(char d)
{
 char* str = malloc(sizeof(char)*2);

 str[0] = d;
 str[1] = '\0';
 
 return str;
}


int nbCouleursJustes(int* couleurs, char* choix, int difficulte){
	int nbJustes = 0;

	int* checkTab = malloc(sizeof(int) * difficulte);
	memset(checkTab, 0, difficulte);

	//on check les pionts bien placés
	for(int i=0; i<difficulte; i++){
		int couleurCourante = 0;
		if(i%2==0)	
			couleurCourante = choix[i/2] && 0x0F;
		else
			couleurCourante = choix[i/2]  >> 4;
		if(couleurs[i]==couleurCourante){
			nbJustes++;
			checkTab[i]=1;
		}
	}

	int nbCouleurs = 0;

	//on check les couleurs présentes
	for(int i=0; i<difficulte; i++){
		if(!checkTab[i]){
			int couleurCourante = 0;
			if(i%2==0)	
				couleurCourante = choix[i/2] & 0x0F;
			else
				couleurCourante = choix[i/2]  >> 4;
			for(int j=0; j<difficulte; j++){
				if(!checkTab[j]){
					if(couleurs[i]==couleurCourante){
						nbCouleurs++;
						checkTab[j]=1;
						break;
					}
				}
			}
		}
	}
	
	return nbJustes << 4 + nbCouleurs; 
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

	//nbJustes contient le nombre de couleurs présentes et de pions bien placés dans un char
	char nbJustes = (char) nbCouleursJustes(couleurs, buffer, difficulte);

	//on envoie le nombre de choix justes au client
	write(socketClient, char_to_string(nbJustes), 2);

	if(nbJustes==difficulte)
		return 1;
	return 0;
}


void jeu(int socketClient){

	char* buffer = malloc(sizeof(char)*2);

	//debut de communication, le client doit envoyer un int correspondant à la difficulté

	int nbRead = h_reads(socketClient, buffer, 2);
	int difficulte = atoi(buffer);

	printf("difficulté : %d \n", difficulte);

	if(difficulte<1 || difficulte >8)
		exit(1);

	printf("tirage des couleurs \n");

	int* couleurs = tirerCouleurs(difficulte);

	printf("affichage des couleurs : \n");

	for(int i =0; i<difficulte; i++)
		printf("%d ", couleurs[i]);

	printf("\n debut des tours \n");

	int nbTour = 1;
	int gagne = 0;
	while( !gagne && nbTour !=12){
		gagne = tour(socketClient, couleurs, difficulte);
		nbTour++;
	}
	
	h_close(socketClient);
	exit(1);
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
	}

	}
}

/******************************************************************************/	
