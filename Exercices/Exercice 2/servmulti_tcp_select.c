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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 1500
int str_echo (int socket)
{
  int nrcv, nsnd;
  char msg[BUFSIZE];

  /*    * Attendre  le message envoye par le client 
   */
  memset( (char*) msg, 0, sizeof(msg) );
  if ( (nrcv= read ( socket, msg, sizeof(msg)-1) ) < 0 )  {
    perror ("servmulti : : readn error on socket");
    exit (1);
  }
  if (nrcv==0)
  {
    // Le message est vide, alors la connection est coupée 
    printf("servmulti : 1 connection coupée \n");
    
    return(2);

  }
  msg[nrcv]='\0';
  printf ("servmulti :message recu=%s nrcv = %d \n",msg, nrcv);

  if ( (nsnd = write (socket, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }
  printf ("nsnd = %d \n", nsnd);
  return (nsnd);
} /* end of function */


usage(){
  printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[])

{
  int i;
  int servSocket, n, newsocket, childpid, servlen,fin;
  struct sockaddr_in  serv_addr, cli_addr;
  socklen_t clilen;
  fd_set readSocket;
  //fd_set allSocket;
  int max_client = 50;
  int clientSocket[max_client];
  int max_fd, sd;
 /* Verifier le nombre de paramètre en entrée */
  if (argc != 2)
  {
    usage();
    exit(1);
  }
  for (i=0;i<max_client;i++) 
  { /* init de la liste des clients */ 
    clientSocket[i] = 0;
  }

/*
 * Ouvrir une socket (a TCP socket)
 */
  if ((servSocket = socket(PF_INET, SOCK_STREAM, 0)) <0) 
  {
    perror("servmulti : Probleme socket\n");
    exit (2);
  }

 
  /*
   * Lier l'adresse  locale à la socket
   */
  memset( (char*) &serv_addr,0, sizeof(serv_addr) );
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
 
 
  if (bind(servSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
    perror ("servmulti : erreur bind\n");
    exit (1);
  }

  /* Paramètrer le nombre de connexion "pending" */
  if (listen(servSocket, SOMAXCONN) <0) 
  {
    perror ("servmulti : erreur listen\n");
    exit (1);
  }
  clilen = sizeof(cli_addr);
  for (;;) {
   
    /*------------------ Init des socket en lecture, tous à zéros --------------------*/ 
    FD_ZERO(&readSocket);
    /* Le socket du serveur est obligatoirement en écoute , on l'ajoute aux socket à surveiller */
    FD_SET(servSocket,&readSocket);
    /* On cherche le max des sockets ouverts */
    max_fd = servSocket;
    for (i = 0; i < max_client ; i++) 
    {
      sd = clientSocket[i];
      if (sd>0) //le socket est ouvert
      {
        /* On le remet dans readSocket, auparavant clean */
        FD_SET(sd,&readSocket);
      }
      if (sd>max_fd) /* recherche du max */
      {
        max_fd = sd;
      }
    }
    /* ------------------------- Initialisation finie --------------------------------*/
    /* ----------- On va faire le select pour voir les sockets qui ont bougé ---------*/
    //printf("Valeur de max fd : %d\n", max_fd);
    if (select(max_fd+1, &readSocket, NULL, NULL, NULL)<0) //dans le select, on doit mettre le max+1
    {
      printf("servmulti : erreur select\n");
    }
    
    /* Un cas important : activité sur le socket serveur */

    if (FD_ISSET(servSocket,&readSocket)) // Le cas  il y a eu de l'activité sur le socket serveur -> nouvelle connection
    {
      newsocket = accept(servSocket, (struct sockaddr *) &cli_addr,  &clilen); 
      if (newsocket < 0) {
        perror("servmulti : erreur accept\n");
        exit (1);
      }
      /* On detecte une nouvelle conection effective */
      printf("servmulti : Nouvelle connection de %s\n", inet_ntoa(cli_addr.sin_addr));
      for (int i = 0; i < max_client; i++)
      {
        if (clientSocket[i]==0)
        { /* On ajoute le socket dès qu'on trouve une place dans la liste des socket ouverts */
          clientSocket[i]=newsocket;
          printf("servmulti : correctement ajoutée\n");
          break;
        }
      }
    }

    /* ---On traite maintenant toutes les arrivées de messages sur les sockets ouverts---*/
    
    for (i = 0; i < max_client; ++i)
    {
      sd = clientSocket[i];
      if (FD_ISSET(sd,&readSocket))
      { /* Si le socket a de l'activité */
        if ( str_echo(sd) == 2)
        {
          close(sd);
          clientSocket[i] = 0;
        }
          
      }
    }
  }
}



















