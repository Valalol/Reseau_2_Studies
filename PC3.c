#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "Liste.h"


#define UDP_port_IN 8002
#define UDP_port_OUT 8000
#define IP_addr_S "127.0.0.1"
#define TAILLE_ETAT_TRAME 1
#define TAILLE_ADRESSE_SOURCE 1
#define TAILLE_ADRESSE_DESTINATAIRE 1
#define TAILLE_MESSAGE 250
#define ADRESSE "3"
#define FILTER "%1s%1s%1s%250[^\n]"

typedef struct paquet
{
	char etat_trame[TAILLE_ETAT_TRAME+1];
	char source[TAILLE_ADRESSE_SOURCE+1];
	char destinataire[TAILLE_ADRESSE_DESTINATAIRE+1];
	char message[TAILLE_MESSAGE+1];
} Paquet;


// Fonction de traitement des paquet reçus
void traiter_trame(Paquet* paquet, SList* Lmessages) {
	// Affichage de Debug
	printf("\nPaquet received : \n - Etat trame : %s\n - Source : %s\n - Destinataire : %s\n - Contenu : %s\n\n", paquet->etat_trame, paquet->source, paquet->destinataire, paquet->message);
	printf("Paquet reçu\n");

	// Si le paquet est un message de broadcast
	if (!strcmp(paquet->etat_trame, "2")) {
		// Si nous sommes l'auteur de ce message, on l'arrete en remettant l'etat trame à 0
		if (!strcmp(paquet->destinataire, ADRESSE)) {
			printf("Message de broadcast reçu par tout l'anneau\n");
			strcpy(paquet->etat_trame, "0");
		}
		// Sinon on le lit et on le laisse passer sans modification
		else {
			printf("Message de broadcast reçu par %s de %s : %s \n", ADRESSE, paquet->source, paquet->message);
		}
	}
	// Si le paquet est un message
	else if (!strcmp(paquet->etat_trame, "1")) {
		// Si le paquet nous est destiné on l'affiche et on change l'etat trame à 0 (token)
		if (!strcmp(paquet->destinataire, ADRESSE)) {
			printf("Message reçu par %s de %s : %s \n", ADRESSE, paquet->source, paquet->message);
			strcpy(paquet->etat_trame, "0");
		}
	}
	// Si le paquet est un token
	else {
		// Si on souhaite envoyer un message.
		if (GetFirstElement(Lmessages)) {
			// si on est le dernier à avoir envoyer un message, on attend un tour pour ne pas monopoliser le réseau et on change la source 0
			if (!strcmp(paquet->source, ADRESSE)) {
				strcpy(paquet->source, "0");
			}
			// Si on peut envoyer un message, on récupère le premier élément de la file, et on modifie notre paquet (destinataire et contenu)
			else {
				SCell* cell = GetFirstElement(Lmessages);
				Message* nouveau_message = GetData(cell);
				DeleteCell(Lmessages, cell);
				
				if (!strcmp(nouveau_message->destinataire, ADRESSE)) {
					strcpy(paquet->etat_trame, "2");
				} else {
					strcpy(paquet->etat_trame, "1");
				}
				strcpy(paquet->source, ADRESSE);
				strcpy(paquet->destinataire, nouveau_message->destinataire);
				strcpy(paquet->message, nouveau_message->contenu);

				free(nouveau_message);
			}
		}
	}
	return;
}

int main() {
	int sock_S, sock_C;

	struct sockaddr_in sa_S, sa_C;
	
	unsigned int taille_sa = sizeof( struct sockaddr );

	int taille_buffer = TAILLE_MESSAGE+TAILLE_ADRESSE_SOURCE+TAILLE_ADRESSE_DESTINATAIRE+TAILLE_ETAT_TRAME;

	char buffer[taille_buffer];

	// typedef struct
    SList* Lmessages = CreateList();

    Message* message = (Message*) malloc(sizeof(Message));
	strcpy(message->destinataire, "1");
	strcpy(message->contenu, "Message 1 de PC 3 destiné à PC 1");

	AddElementEnd(Lmessages, message);


	sock_S = socket(PF_INET, SOCK_DGRAM, 0);
	sock_C = socket(PF_INET, SOCK_DGRAM, 0);

	bzero((char*) &sa_C, sizeof(struct sockaddr));
	sa_C.sin_family      = AF_INET;
	sa_C.sin_addr.s_addr = htonl( INADDR_ANY );
	sa_C.sin_port        = htons( UDP_port_IN );

	bzero((char*) &sa_S, sizeof(sa_S));
	sa_S.sin_family      = AF_INET;
	sa_S.sin_addr.s_addr = inet_addr( IP_addr_S );
	sa_S.sin_port        = htons( UDP_port_OUT );

	bind(sock_S, (struct sockaddr *) &sa_C, sizeof(struct sockaddr));

	Paquet* paquet = (Paquet*) malloc(sizeof(Paquet));

	while(1)
	{
		memset (buffer, '\0', sizeof(buffer));

		recvfrom(sock_S, buffer, taille_buffer * sizeof(char), 0, (struct sockaddr *) &sa_C, &taille_sa);
		
		/* affichage */
		// printf("%s \n", buffer);
		sscanf(buffer, FILTER, paquet->etat_trame, paquet->source, paquet->destinataire, paquet->message);

		traiter_trame(paquet, Lmessages);

		sprintf(buffer, "%s%s%s%s", paquet->etat_trame, paquet->source, paquet->destinataire, paquet->message);
		sleep(2);

		/* re-emission datagramme vers client */
		sendto(sock_C, buffer, taille_buffer * sizeof(char), 0, (struct sockaddr*) &sa_S, taille_sa);
	}

	/* fin */
	free(Lmessages);
	close(sock_S);
	close(sock_C);
	exit(EXIT_SUCCESS);
}

