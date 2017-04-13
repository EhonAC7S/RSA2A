


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
  //struct hostent *hp;  
  int ttl;	
  socklen_t len=sizeof(multi_addr);
  int sendPID;
  struct ip_mreq group;
  u_int yes=1;
  /* Verifier le nombre de paramètre en entrée */
  /* clientUDP <hostname> <numero_port>        */
  if (argc != 3)
  {
    usage();
    exit(1);
  }
 
  //  bcopy( (char *) hp->h_addr,  (char *)& serv_addr.sin_addr, hp->h_length);
  //memcpy( & multi_addr.sin_addr ,  hp->h_addr,  hp->h_length);
  //printf ("IP address: %s\n", inet_ntoa (multi_addr.sin_addr));
   
  /*
   * Ouvrir socket UDP
   */
  
  if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) <0) 
  {
    perror ("erreur socket");
    exit (1);
  }

   /* 
    * Remplir la structure  serv_addr avec l'adresse de multicast 
    */
  memset( (char *) &multi_addr,0, sizeof(multi_addr) );
  multi_addr.sin_family = AF_INET;
  multi_addr.sin_addr.s_addr = inet_addr(argv[1]);
  multi_addr.sin_port = htons(atoi(argv[2]));

  /* Option spécifiques au multicast */
  ttl=1;
  if (setsockopt(serverSocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) <0)
  {
    perror ("setsockopt");
    exit (1);
  }
  /*if (setsockopt(serverSocket, IPPROTO_IP, IP_MULTICAST_, &multi_addr, sizeof(multi_addr)) <0)
  {
    perror ("setsockopt");
    exit (1);
  }*/
  if (setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) 
  {
       perror("Reusing ADDR failed");
       exit(1);
  }


  for (;;)
  {  

    /* On fait un fork pour avoir 2 processus parallèles, un qui envoie, l'autre qui reçoit */
    if ( (sendPID = fork() ) < 0) 
    {
      perror ("server: fork error\n");
      exit (1);
    }
    else
    { /* Si on a pas eu de problème avec le fork() */
      if (sendPID == 0) 
      { /* Version où on scanf ce que le client veut envoyer aux autres clients */
        /* On ne prend pas cette version pour l'instant car il est demandé d'envoyer l'heure */     
        /*
        wait(NULL);
        data = "";
        scanf("%s",data);
        if (data[0]=='q' && data[1]==' ') 
        {
          close(serverSocket);
          exit(1);
        }
        */
        /* Version où on envoie toutes les 3 secondes la dates aux autres clients connectées */
        //wait(NULL); // On attend que le fils soit fini (s'il y en a eu un avant)
        ftime(&tp);
        data =ctime(&tp.time); // on stock l'heure
        if ( (n = sendto( serverSocket, data, strlen(data), 0, (struct 
          sockaddr *) &multi_addr,sizeof(multi_addr))) <0)  
        { //On verifie qu'elle a été correctement envoyée
          perror ("erreur sendto");
          //exit (1);
        }
        sleep(3);
        exit(0);
      }
      else
      { /* cas classique de reception */
        if ((n=recvfrom(serverSocket,sendbuf,sizeof(sendbuf)-1,0,
        (struct sockaddr *)&multi_addr, &len)) <0 )
        {
          printf ("erreur recvfrom");
          exit (1);
        }
        sendbuf[n]='\0';
        printf ("Message reçu : %s \n", sendbuf);
      }
    } 
  }
}
