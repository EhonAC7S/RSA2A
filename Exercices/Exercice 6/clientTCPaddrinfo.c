/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client) pour un protocole      *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     connect()                                                     *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *********************************************************************/
#include      <stdio.h>
#include      <sys/types.h>
#include      <sys/socket.h>
#include      <netinet/in.h>
#include      <string.h>
#include      <arpa/inet.h>
#include      <strings.h>
#include      <unistd.h>
#include      <netdb.h>
#include      <stdlib.h>

#define MAXLINE 80

int writen (int  fd, char *ptr, int nbytes)
{
  int nleft, nwritten; 

  nleft = nbytes;
  while (nleft >0) {
    nwritten = write (fd,ptr, nleft);
    if (nwritten <=0) {
      if(errno == EINTR)
    nwritten=0;
      else{
    perror("probleme  dans write\n");
    return(-1);
      }
    }
    nleft -= nwritten;
    ptr += nwritten;
  }
  return (nbytes);
}

/*
 * Lire  une ligne terminee par \n à partir d'un descripteur de socket
 */
int readline (int  fd, char *ptr, int maxlen)
{
  
  int n, rc, retvalue, encore=1;  char c, *tmpptr; 

  tmpptr=ptr;
  for (n=1; (n < maxlen) && (encore) ; n++) {
    if ( (rc = read (fd, &c, 1)) ==1) {
      *tmpptr++ =c; 
      if (c == '\n')  /* fin de ligne atteinte */
    {
      encore =0; retvalue = n;
    }
    }else if (rc ==0) {  /* plus rien à lire */
      encore = 0;
      if (n==1) retvalue = 0;  /* rien a été lu */
      else retvalue = n;
    }
    else { /*rc <0 */
      if (errno != EINTR) {
    encore = 0;
    retvalue = -1;
      }
    }
  }
  *tmpptr = '\0';  /* pour terminer la ligne */
  return (retvalue);
}

usage(){
  printf("usage : cliecho adresseIP_serveur(x.x.x.x)  numero_port_serveur\n");
}




int main (int argc, char *argv[])

{
  int serverSocket, servlen, n, retread, err;
  char fromServer[MAXLINE];
  char fromUser[MAXLINE];
  //struct sockaddr_in6  serv_addr; n'est plus utile avec la struct addrinfo
  //struct hostent *hp;  
  struct addrinfo base; //Structure addrinfo que nous allons exploiter
  struct addrinfo *res;

  /* Verifier le nombre de paramètre en entrée */
  /* clientTCP <hostname> <numero_port>        */
  if (argc != 3){!
    usage();
    exit(1);
    }

  base.ai_family = PF_INET6; //Choix de la famille de base
  base.ai_socktype = SOCK_STREAM //En connection TCP
  // La suite des element de base est initialisée à zéro pour ne pas avoir de surprise
  base.ai_flags = 0;
  base.ai_protocol = 0;
  base.ai_addrlen = 0;
  base.ai_addr = NULL;
  base.ai_canonname = NULL;
  base.ai_next = NULL;
  /* 
   * Remplir la structure  serv_addr avec l'adresse du serveur 
   */
  /*****Le bloc suivant en commentaire est simplement remplacé par un appel bien choisi de getaddrinfo()***/
  
  //memset ( (char *) &serv_addr, 0, sizeof(serv_addr) );
  //serv_addr.sin6_family = AF_INET6;                         //Changement ici
  //serv_addr.sin6_port = htons(atoi(argv[2]));               //Changement ici
  //hp = (struct hostent *)gethostbyname2 (argv[1],AF_INET6); //Changement ici
  //if (hp == NULL) {
  //  fprintf(stderr, "%s: %s non trouve dans in /etc/hosts ou dans le DNS\n",
  //          argv[0], argv[1]);
  //  exit(1);
  //}
  //serv_addr.sin6_addr = * ((struct in_addr *)(hp->h_addr)); //Changement ici
  //printf ("IP address: %s\n", inet_ntoa (serv_addr.sin6_addr));
  // 
   
  err = getaddrinfo(argv[1], argv[2], &criteres, &res); //On va tout simplement chercher les infos sur le serveur avec lequel on communique

  if(err){
    perror("Erreur getaddrinfo");
    exit(1);
  }

  /*
   * Ouvrir socket (socket STREAM)
   */
  if ((serverSocket = socket(base.ai_family, base.ai_socktype, base.ai_protocol)) <0) { //Changement ici
    perror ("Erreur socket");
    exit (1);
  }

  /*
   * Connect to the serveur 
   */
  if (connect (serverSocket, (struct sockaddr *) &serv_addr,  
	       sizeof(serv_addr) ) < 0){
     perror ("erreur connect");
    exit (1);
  }

  while ( (retread =readline (serverSocket, fromServer, MAXLINE)) > 0)
    {
      printf ("corr: %s", fromServer);
      if (strcmp (fromServer,"Au revoir\n") == 0) 
	break ; /* fin de la lecture */
      
      /* saisir message utilisateur */
      printf("vous: ");
      if (fgets(fromUser, MAXLINE,stdin) == NULL) {
	perror ("erreur fgets \n");
	exit(1);
      }
      
      /* Envoyer le message au serveur */
      if ( (n= writen (serverSocket, fromUser, strlen(fromUser)) ) != strlen(fromUser))  {
	printf ("erreur writen");
	exit (1);
      }
    }
  if(retread < 0 ) {
    	perror ("erreur readline \n");
	//	exit(1);
  }

  close(serverSocket);
}
