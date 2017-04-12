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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define BUFSIZE 1500
int str_echo (int sockfd)
{
  int nrcv, nsnd;
  char msg[BUFSIZE];

  /*    * Attendre  le message envoye par le client 
   */
  memset( (char*) msg, 0, sizeof(msg) );
  if ( (nrcv= read ( sockfd, msg, sizeof(msg)-1) ) < 0 )  {
    perror ("servmulti : : readn error on socket");
    exit (1);
  }
  msg[nrcv]='\0';
  printf ("servmulti :message recu=%s du processus %d nrcv = %d \n",msg,getpid(), nrcv);

  if ( (nsnd = write (sockfd, msg, nrcv) ) <0 ) {
    printf ("servmulti : writen error on socket");
    exit (1);
  }
  printf ("nsnd = %d \n", nsnd);
  if (nsnd==0)
  {
    printf("servmulti : Connection coupée avec %d\n", sockfd);
    
  }
  return (nsnd);
} /* end of function */


usage(){
  printf("usage : servmulti numero_port_serveur\n");
}


int main (int argc,char *argv[])

{
  int sockfd, n, newsockfd, childpid, servlen,fin; 
  struct sockaddr_in  serv_addr, cli_addr;
  socklen_t clilen; //nombre de client
  fd_set descList, readDesc; //liste de descripteur, tous les descripteurs et ceux en read
  FD_ZERO(&descList);
  FD_ZERO(&readDesc);
  /* Verifier le nombre de paramètre en entrée */
  if (argc != 2){
    usage();
    exit(1);
  }
/*
 * Ouvrir une socket (a TCP socket)
 */
  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0) {
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
 
 
  if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
    perror ("servmulti : erreur bind\n");
    exit (1);
  }

  /* Paramètrer le nombre de connexion "pending" */
  if (listen(sockfd, SOMAXCONN) <0) {
    perror ("servmulti : erreur listen\n");
    exit (1);
  }
  /* init avec le premier listenneur*/
  FD_SET(sockfd,&readDesc);
  /*On garde en memoire le dernier desc qui est le premier à l'init*/
  n++; 
  for (;;) {
    /*Tous les desc en lecture*/
    //FD_ZERO(&readDesc);
    /* On fait un select pour voir les changement d'état sur les descripteurs et on test qu'il n'y ait pas d'erreur*/
    if (select(n+1, &readDesc, NULL, NULL, NULL)<0)  
    { //On ne regarde que le descr en read et pas ceux en write ni except
      printf("servmulti : erreur select");
    } 
    printf("Select passed\n");
    //int i;
    /* On parcours tous les descripeurs de readDesc*/
    for (int i = 0; i < n; i++ ) 
    {
      /*On agit s'il est "actif*/
      if (FD_ISSET(i,&readDesc)) 
      {
        /*2 cas possible */ // --------------------------------------------------------------
        if (i == sockfd)
        {
          /* On a une activité depuis le socket initial, alors c'est un nouveau client */
          /* On applique la procedure classique d'ajout */
          clilen = sizeof(cli_addr);
          /* On crée une nouvelle socket */
          newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &clilen);
          if (newsockfd < 0) {
            perror("servmulti : erreur accept\n");
            exit (1);
          }
          /*On stock le nouveau descripteur dans la liste des descripteurs*/
          FD_SET(newsockfd,&readDesc);
          if (newsockfd>n) 
          { /* Le nouveau descripteur devient le dernier */
            n++;
          }
          printf("servmulti : new connection from %s\n", inet_ntoa(cli_addr.sin_addr));
          if ( (childpid = fork() ) < 0) {
            perror ("server: fork error\n");
            exit (1);
          }
          else  
           
            if (childpid == 0) {       
              close (sockfd);
              while (str_echo (newsockfd));  
              exit (0); 
            }
            close(newsockfd); 

        }
          
          
          
      }
       
    }
  }
}



















