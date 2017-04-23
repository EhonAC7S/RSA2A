/*********************************************************************
 *                                                                   *
 * FICHIER: SERVER_TCP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de TCP socket par une application serveur*
 *              application client                                   *
 *                                                                   *
 * principaux appels (du point de vue serveur) pour un protocole     *
 * oriente connexion:                                                *
 *     socket()                                                      *
 *                                                                   * 
 *     bind()                                                        *
 *                                                                   * 
 *     listen()                                                      *
 *                                                                   *
 *     accept()                                                      *
 *                                                                   *
 *     read()                                                        *
 *                                                                   *
 *     write()                                                       *
 *                                                                   *
 *********************************************************************/
#include      <sys/types.h>
#include      <sys/socket.h>
#include      <netinet/in.h>
#include      <arpa/inet.h>
#include      <stdio.h>
#include      <string.h>
#include      <netdb.h>
#include      <stdlib.h>
#include      <string.h>

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
  printf("usage : servecho numero_port_serveur\n");
}


int main (int argc, char *argv[])

{
  int serverSocket, clientSocket; /* declaration socket passive et socket active */

  int n, retread, childpid;
  socklen_t clilen;
  struct sockaddr_in6  serv_addr, cli_addr; //Changement ici   
  char fromClient[MAXLINE];
  char fromUser[MAXLINE];
  struct addrinfo *res,
  struct addrinfo base;
  /* Verifier le nombre de paramètre en entrée */
  /* serverTCP <numero_port>                   */ 
  if (argc != 2){
    usage();
    exit(1);
  }

  memset(&base, 0, sizeof(base));
  base.ai_flags = AI_PASSIVE;
  base.ai_socktype = SOCK_STREAM;
  base.ai_family = PF_INET6;

  //node à NULL = un socket de serveur
  
  if(getaddrinfo(NULL, argv[1], &base, &res)){ //On récupère les infos que l'on veut dans res avec le bon protocole/famille/etc...
    perror("Erreur dans le getaddreinfo);
    exit(1);
  }
   /*
   * Ouvrir une socket (a TCP socket)
   */
  if ((serverSocket = socket(res.ai_family, res.ai_socktype, res.ai_protocol)) <0) { 
   perror("servecho : Probleme socket\n");
   exit (2);
  }
 
  if (bind(serverSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
   perror ("servecho: erreur bind\n");
   exit (1);
  }
  
  /* Paramètrer le nombre de connexion "pending" */
  if (listen(serverSocket, SOMAXCONN) <0) {
    perror ("servecho: erreur listen\n");
    exit (1);
  }


  clilen = sizeof(cli_addr);
  clientSocket = accept(serverSocket, (struct sockaddr *) &cli_addr, &clilen);
  if (clientSocket < 0) {
    perror("servecho : erreur accept\n");
    exit (1);
  }


  /* Envoyer Bonjour au client */
  if ( (n= writen (clientSocket, "Bonjour\n", strlen("Bonjour\n")) ) != strlen("Bonjour\n"))  {
	printf ("erreur writen");
	exit (1);
     }

  while ( (retread=readline (clientSocket, fromClient, MAXLINE)) > 0)
    {
      printf ("corr: %s", fromClient);
      if (strcmp (fromClient,"Au revoir\n") == 0) 
	break ; /* fin de la lecture */
      
      /* saisir message utilisateur */
      printf("vous: ");
      if (fgets(fromUser, MAXLINE,stdin) == NULL) {
	perror ("erreur fgets \n");
	exit(1);
      }
      
      /* Envoyer le message au client */
      if ( (n= writen (clientSocket, fromUser, strlen(fromUser), 0) ) != strlen(fromUser))  {
	printf ("erreur writen");
	exit (1);
     }

    }

  if(retread < 0 ) {
    perror ("erreur readline \n");
    //   exit(1);
  }
  close(serverSocket);
  close(serverSocket);
  

}
