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


void communicatationClient(int socketClient){
	char* buffer = malloc(sizeof(char)*256);
	int nbRead = h_reads(socketClient, buffer, 256);
	write(0, buffer, nbRead);
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
		communicatationClient(client);
	}

	}
}

/******************************************************************************/	
