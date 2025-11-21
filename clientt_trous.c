// clientt.c (client TCP)

#include <stdio.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>


#define NBECHANGE 3

char* id=0;
short sport=0;

int sock=0;	/* socket de communication */

int main(int argc, char** argv)
{
struct  sockaddr_in moi; /* SAP du client */
struct  sockaddr_in serveur; /* SAP du serveur */
int nb_question=0; 
int ret,len;

	if (argc!=4) {
		fprintf(stderr,"usage: %s id serveur port\n",argv[0]);
		exit(1);
	}
	id= argv[1]; //identifiant du client
	sport= atoi(argv[3]); //convertion int du port serveur

	if ((sock = socket (AF_INET,SOCK_STREAM,0)) == -1) { // Utilise en socket en IPv4 et TCP
		fprintf(stderr,"%s: socket %s\n",argv[0],
			strerror(errno));
		exit(1);
	}
	serveur.sin_family = AF_INET; 
	serveur.sin_port = htons(sport); //inversion des octets du port
	inet_aton(argv[2],&serveur.sin_addr); //argv[2] est l'adresse IP de destination qui est transmise à serveur.sin_addr
	
	if (connect(sock,(struct sockaddr *)&serveur,sizeof(serveur))<0) { //se connect au serveur
		fprintf(stderr,"%s: connect %s\n",argv[0],strerror(errno));
		perror("bind");
		exit(1);
	}
	len=sizeof(moi); 
	getsockname(sock,(struct  sockaddr  *)&moi,&len); //Donne l'adresse IP du socket au client

	for (nb_question=0 ; nb_question<NBECHANGE ; nb_question++) {
		char    buf_read[1<<8], buf_write[1<<8]; //[1<<8] soit 256 octets

		sprintf(buf_write,"#%2s=%03d",id,nb_question); //stocke dans buf write le messsage à envoyer qui est de la forme #id=numero_question soit pour nb_question=0 "#id=000"
		printf(" client %2s: (%s:%4d) envoie a  ", id,
			inet_ntoa(moi.sin_addr),ntohs(moi.sin_port)); // met le message : "client $id: (IP:port) envoie a "
		printf("(%s:%4d) : %s\n",
			inet_ntoa(serveur.sin_addr),ntohs(serveur.sin_port),
			buf_write); // met le message "(IP_serveur:port_serveur) : buf_write"
        ret=write(sock,buf_write,strlen(buf_write)); //envoie le message buf_write au serveur
        if (ret != strlen(buf_write)) { // CHANGEMENT ICI: on vérifie si ret est DIFFERENT de la longueur
            printf("\n%s: erreur dans write (num=%d, mess=%s)\n",
                argv[0],ret,strerror(errno));
            continue;
        }
		printf(" client %2s: (%s:%4d) recoit de ",id,  //affiche le message "client $id: (IP:port) recoit"
				inet_ntoa(moi.sin_addr),ntohs(moi.sin_port));
		ret=read(sock,buf_read,sizeof(buf_read)); //lit le message reçu du serveur
		if (ret<=0) 
		{ //vérifie que le message a bien été reçu
			printf("\n%s:  erreur dans read (num=%d, mess=%s)\n",
				argv[0],ret,strerror(errno));
			continue;
		}
		printf("(%s:%4d) : %s\n",inet_ntoa(serveur.sin_addr), // met le message "(IP_serveur:port_serveur) : buf_read"
		ntohs(serveur.sin_port),buf_read);
	}
	close(sock); 
	printf(" j ai fini, bye\n");
	return 0;
}
