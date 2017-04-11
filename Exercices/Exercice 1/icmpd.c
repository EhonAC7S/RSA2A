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
  int rawSocket, n,lenIPHeader, lenIPHeader2, sport, dport;
  struct sockaddr_in  fromAddr;
  socklen_t len; 
  char source[BUFSIZE],rcvbuffer[BUFSIZE];;
  struct ip *ip, *ip2;
  struct icmp *icmp;
  struct udphdr *udp;

  if ((rawSocket = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) <0) { //Opions classiques de mode RAW
    perror ("erreur socket");
    exit (1); }
  
  for (;;) {
    len = sizeof(fromAddr);
    if ( (n= recvfrom (rawSocket, rcvbuffer, BUFSIZE,0, 
		     (struct sockaddr *)&fromAddr, &len)) < 0 )  { //On stock ce qui vient du socket ouvert dans un buffer rcvbuffer
      printf ("erreur recvfrom");
      exit (1);}
    
    if ( inet_ntop (PF_INET, (const void *)&fromAddr.sin_addr,source,sizeof(source)) <0) {
      printf ("erreur inet_ntop");
      exit (1); }
    printf( "%d octets ICMP de %s: \n", n, source);
    printf("%s\n", rcvbuffer);
    //printf("%s\n", source);
    char NewLogName[30];                                  // Bloc d'affichage de l'heure
    time_t rawtime;                                       //
    struct tm * timeinfo;                                 //
    time ( &rawtime );                                    //
    timeinfo = localtime ( &rawtime );                    //
    char s[100];                                          //
    strftime(s,30,"%b.%d.%Y - %H.%M",timeinfo);           //
    printf("%s\n",s);                                     //
  
    ip =  (struct ip*) rcvbuffer ;                           // debut entete IP //voir structure du paquet ICMP
    
    lenIPHeader = ip->ip_hl * 4 ;       // ip->ip_hl longueur  en mot de 32 bits //On recupère le nombre de mots et on multiplie pas 4 octects == 32bits
  
    icmp =  (struct icmp*) (rcvbuffer + lenIPHeader) ;             // debut entete ICMP // le paquet ICMP se trouve après le paquet IP dans le lot d'info reçues
    ip2 =  &(icmp->icmp_ip) ;              // debut en-tete IP contenu dans ICMP //
    lenIPHeader2 = ip2->ip_hl * 4 ;       // longueur  en-tete IP  //Même formule qu'au dessus  
    printf("%s\n", rcvbuffer+lenIPHeader+lenIPHeader2 );
    if (ip2->ip_p == IPPROTO_UDP) {
      udp = (struct udphdr*) (ip2 + lenIPHeader2);  // debut en-tete UDP //On trouve le headerUDP apres le headerIP
      sport = ntohs(udp->source);
      dport = ntohs(udp->dest) ;
      printf (" port source = %d et port destination = %d \n", sport, dport);
    }
    switch (icmp->icmp_type) {
    case ICMP_UNREACH: {
      printf ("destination unreachable \n");
      switch (icmp->icmp_code){
      case ICMP_UNREACH_PORT:
	printf (" bad port \n");
	break;
      default: 
	printf ("type %d, code = %d\n", icmp->icmp_type,
	   icmp->icmp_code);
	break;
      } }
      break;
    case ICMP_ECHO:
      printf (" echo service  \n");
      break;
    case ICMP_ECHOREPLY :
      printf (" echo reply  \n");
      break;
    case ICMP_TIMXCEED :
      printf (" Time Exceed \n");
      break;
    default:
      printf ("type %d, code = %d\n", icmp->icmp_type,
	   icmp->icmp_code);
    }
  }
}
