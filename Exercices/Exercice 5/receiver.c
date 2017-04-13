


/*********************************************************************
 *                                                                   *
 * FICHIER: CLIENT_UDP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de UDP socket par une application client *
 *                                                                   *
 * principaux appels (du point de vue client)                        *
 *     socket()                                                      *
 *                                                                   * 
 *     sendto()                                                      *
 *                                                                   *
 *     recvfrom()                                                    *
 *                                                                   *
 *********************************************************************/
#include     <stdio.h>
#include     <sys/types.h>
#include     <sys/socket.h>
#include     <netinet/in.h>
#include     <sys/un.h>
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     <strings.h>
#include     <unistd.h>
#include     <sys/wait.h>
#include     <unistd.h>

#define MAXLINE 80

usage()
{
  printf("usage : cliecho adresseIP_multicast(224.x.x.x)  numero_port\n");
}

int main (int argc, char *argv[])

{

  int serverSocket, n;
  struct sockaddr_in  multi_addr;
  char *data;
  char sendbuf[1500];
  int buffLen = sizeof(sendbuf);
  
  struct timeb tp;
  u_int yes=1;
  int ttl;
  //socklen_t len=sizeof(multi_addr);
  int sendPID;
  struct ip_mreq group;

  /* Verifier le nombre de paramètre en entrée */
  /* clientUDP <hostname> <numero_port>        */
  if (argc != 3)
  {
    usage();
    exit(1);
  }
  /*
   * Ouvrir socket UDP
   */
  
  if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) <0) 
  {
    perror ("erreur socket");
    exit (1);
  }
  /* On veut pouvoir mettre plusieurs  socket sur le même port */
  if (setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) 
  {
    perror("Reusing ADDR failed");
    exit(1);
  }

   /* 
    * Remplir la structure  multi_addr avec l'adresse de multicast 
    */
  memset( (char *) &multi_addr,0, sizeof(multi_addr) );
  multi_addr.sin_family = AF_INET;
  multi_addr.sin_addr.s_addr = inet_addr(argv[1]);
  //multi_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
  multi_addr.sin_port = htons(atoi(argv[2]));
  /* On affecte la socket à l'espace des noms */
  if (bind(serverSocket,(struct sockaddr *) &multi_addr,sizeof(multi_addr)) < 0) 
  {
    perror("bind");
    exit(1);
  }
  /* Paramétrage du groupe */
  group.imr_multiaddr.s_addr=inet_addr(argv[1]);
  group.imr_interface.s_addr=htonl(INADDR_ANY);
  /* On dit au système que la socket rejoind le groupe */
  if (setsockopt(serverSocket,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group)) < 0) 
  {
    perror("setsockopt");
    exit(1);
  }

  socklen_t len = sizeof(multi_addr);
  for (;;)
  {  
    
    if ( (n=recvfrom(serverSocket, sendbuf, sizeof(sendbuf), 0,(struct sockaddr *) &multi_addr,&len)) <0)  
    { //On verifie qu'elle a été correctement envoyée
      perror ("erreur sendto");
      //exit (1);
    }
    sendbuf[n]='\0';
    printf("received : %s\n", sendbuf);
    
  }
}
