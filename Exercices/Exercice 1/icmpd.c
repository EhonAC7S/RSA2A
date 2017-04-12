/*********************************************************************
 *                                                                   *
 * FICHIER: DAEMON ICMP                                               *
 *                                                                   *
 * DESCRIPTION: Utilisation de la  socket raw			*
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
#include     <arpa/inet.h>
#include     <sys/uio.h>
#include     <time.h>
#include     <sys/timeb.h>
#include     <netdb.h>
#include     <stdlib.h>
#include     <strings.h>
#include     <string.h>
#include     <netinet/ip.h>
#include     <netinet/ip_icmp.h>
#include     <netinet/udp.h>

 /****
   **** STRUCTURE ICMP
   ****/
#define BUFSIZE 1500  /* Taille du MTU */
#define MAXLINE 80




int main (int argc, char *argv[])

{
  int rawSocket, n, m, lenIPHeader, lenIPHeader2, sport, dport;
  struct sockaddr_in  serv_addr, fromAddr;
  socklen_t len; 
  char source[BUFSIZE],rcvbuffer[BUFSIZE];;
  struct ip *ip, *ip2;
  struct icmp *icmp;
  struct udphdr *udp;

  /* On créer une socket */

  if ((rawSocket = socket(PF_INET, SOCK_DGRAM, 0)) <0) 
  { 
    //Opions classiques de mode RAW
    perror ("erreur socket");
    exit (1); 
  }
  /* On memorise l'adresse du serveur */
  memset( (char*) &serv_addr,0, sizeof(serv_addr) );
  serv_addr.sin_family = PF_INET;
  serv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
  
  /* On associe la socket à une adresse, celle du serveur */
  
  if (bind(rawSocket,(struct sockaddr *)&serv_addr, sizeof(serv_addr) ) <0) {
    perror ("erreur bind\n");
    exit (1);
  }
  /* On lance le serveur puisqu'il est configuré */ 
  for (;;) 
  {
    len = sizeof(fromAddr);
    if ( (n= recvfrom (rawSocket, rcvbuffer, BUFSIZE,0, (struct sockaddr *)&fromAddr, &len)) < 0 )  
    { //On stock ce qui vient du socket ouvert dans un buffer rcvbuffer
      printf ("erreur recvfrom");
      exit (1);
    }
    if ( inet_ntop (PF_INET, (const void *)&fromAddr.sin_addr,source,sizeof(source)) <0) 
    {
      printf ("erreur inet_ntop");
      exit (1); 
    }
    /* 
     * On fait de l'affichage (peut etre qu'il faut décortiquer 
     * le message et pas l'afficher en entier) En fait pas besoin après test.
     */
    
    printf( "%d octets ICMP de %s: \n", n, source);
    /* 
     * On rajoute un caractère de fin de chaine dans rcvbuffer 
     * sinon on affiche du caca dans le terminal 
     * (espace mémoire lu mais on sait pas ce qu'il vaut) 
     */
    rcvbuffer[n] = '\0';
    printf("message reçu : %s\n", rcvbuffer);

     /* On prévoit le renvoie du même message au client */

    if ( (m= sendto (rawSocket, rcvbuffer, n,0, 
      (struct sockaddr *)&fromAddr, sizeof(fromAddr))) != n)  
    {
      perror ("erreur sendto");
      exit (1);
    }
    
  }
}
