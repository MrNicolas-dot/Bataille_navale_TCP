//serveurt.c (serveur TCP)

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>


#define NBECHANGE 3

char* id=0;
short port=0;

int sock=0;	/* socket de communication */

int  nb_reponse=0;

int main(int argc, char** argv)
{
struct  sockaddr_in serveur; /* SAP du serveur */

	if (argc!=3) {
		fprintf(stderr,"usage: %s id port\n",argv[0]);
		exit(1);
	}
	id= argv[1]; //identifiant du serveur
	port= atoi(argv[2]); //port du serveur en int

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) == -1) { //ouvre un socket en IPv4, TCP
		fprintf(stderr,"%s: socket %s\n",argv[0],strerror(errno));
		exit(1);
	}

	serveur.sin_family = AF_INET;

	serveur.sin_port = htons(port);
	serveur.sin_addr.s_addr = INADDR_ANY;//INADDR_ANY est égal à 0.0.0.0 cequi veut dire qu'il accepte toutes les adresses IP

	if (bind (sock, (struct sockaddr *)&serveur,sizeof(serveur))<0) {//organise la liaison entre le socket et l'adresse IP/port
		fprintf(stderr,"%s: bind %s\n",argv[0],strerror(errno));
		exit(1);
	}

   if (listen (sock,5)!=0) { //file d'attente de 5 clients
		fprintf(stderr,"%s: listen %s\n",argv[0],strerror(errno));
		exit(1);
   }

	while (1) 
	{
		struct  sockaddr_in client; /* SAP du client */
		int     len=sizeof(client); 
		int     sock_pipe;          /* socket de dialogue */
		int     ret,nb_question;

    	sock_pipe = accept(sock, (struct sockaddr *)&client,&len); //Attend une réponse d'un client et crée un nouveau socket de dialogue
		for ( nb_question=0 ; nb_question<NBECHANGE ; nb_question++) 
		{
			char buf_read[1<<8], buf_write[1<<8];

			ret=read(sock_pipe,buf_read,sizeof(buf_read)); //lit le message envoyé par le client
			if (ret<=0) 
			{
				printf("%s: read=%d: %s\n",argv[0],ret,
					strerror(errno));
				break;
			}
			printf("serveur %s recu de (%s,%4d) : %s\n",id,
				inet_ntoa(client.sin_addr),
				ntohs(client.sin_port),buf_read);
			sprintf(buf_write,"#%2s=%03d#",id,nb_reponse++);
			ret=write(sock_pipe,buf_write,sizeof(buf_write)); //envoie le message buf_write au client
			if (ret<=0) {
				printf("%s: write=%d: %s\n",argv[0],ret,
					strerror(errno));
				break;
			}
			sleep(2);
		}
		close(sock_pipe);
	}
	return 0;
}
