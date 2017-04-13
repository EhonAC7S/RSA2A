


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
  struct timeb tp;
  
  int ttl;	
  socklen_t len=sizeof(multi_addr);
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

   /* 
    * Remplir la structure  multi_addr avec l'adresse de multicast 
    */
  memset( (char *) &multi_addr,0, sizeof(multi_addr) );
  multi_addr.sin_family = AF_INET;
  multi_addr.sin_addr.s_addr = inet_addr(argv[1]);
  multi_addr.sin_port = htons(atoi(argv[2]));

  for (;;)
  {  
    ftime(&tp);
    data = ctime(&tp.time); // on stock l'heure
    //printf("sizeof data %d\n", sizeof(data));
    if (sendto(serverSocket, data, strlen(data), 0,(struct sockaddr *) &multi_addr,sizeof(multi_addr)) <0)  
    { //On verifie qu'elle a été correctement envoyée
      perror ("erreur sendto");
      //exit (1);
    }
    printf("sent : %s\n", data);
    sleep(3); //toutes les 3 secondes
  }
}
