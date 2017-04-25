
#include "utils/socketutil.h"
#include "utils/util.h"

int main(int argc, char const *argv[])
{
	struct addrinfo *res = NULL;
	int serverSocketIPv4
	int serverSocketIPv6;
	int clientSocket = -1;
	int webSocket = -1;
	int rd;
	char requete[MAXREQUEST], type_requete[MAXENTETE], hostname[MAXHOST];
	char response[MAXRESPONSE];
	fd_set init_set, desc_set;
	int maxfd, nbfd;
	
	if(argc != 2){
		usage();
		exit(1);
	}

	// Initialisation par addrinfo dans le initServeurSocket
	initServer(&res, argv[1]);
	serverSocketIPv4 = createSocket(res);
	res = res->ai_next;
	serverSocketIPv6 = createSocket(res);

	// On écoute sur les deux sockets
	maxfd = openServer(serverSocketIPv4, serverSocketIPv6);

	// Initialisation des descripteurs
	FD_ZERO(&init_set);
	FD_SET(serverSocketIPv4, &init_set);
	FD_SET(serverSocketIPv6, &init_set);
	FD_ZERO(&desc_set);
	showMyIp(res, argv[1]);
	freeaddrinfo(res);

	while(1)
	{

		desc_set = init_set;

		// On attend une connexion
		nbfd = select(maxfd, &desc_set, NULL, NULL, NULL);

		/******* Arrivée d'une nouvelle socket d'écoute *******/

		//On a l'arrivée d'un nouveau client en IPv4 et pas d'autres clients en cours
		if(clientSocket == -1 && FD_ISSET(serverSocketIPv4, &desc_set))
		{
			
			printf("\n==================================\n");
			printf("Arrivée d'un nouveau client en IPv4\n");

			clientSocket = addClient(serverSocketIPv4, &init_set);

			if(clientSocket >= maxfd) 
			{
				maxfd=clientSocket+1;
			}

			nbfd--;
			FD_SET(clientSocket, &init_set);

		}else if(clientSocket == -1 && FD_ISSET(serverSocketIPv6, &desc_set))
		{ //On a reçu un client en IPv6 et pas d'autres clients en cours
			printf("\n==================================\n");
			printf("Arrivée d'un nouveau client en IPv6\n");

			clientSocket = addClient(serverSocketIPv6, &init_set);

			if(clientSocket >= maxfd) 
			{
				maxfd=clientSocket+1;
			}

			nbfd--;
			FD_SET(clientSocket, &init_set);
		}

		/******* Ensuite on vérifie les sockets de dialogue *******/

		/*** on regarde si l'on a une réponse du serveur web ***/
		if(webSocket >= 0 && FD_ISSET(webSocket, &desc_set))
		{
			//On lit la réponse du site web
			memset(response, 0, MAXRESPONSE);
			rd = recv(webSocket, response, MAXRESPONSE, 0);

			if(rd < 0)
			{
				perror("Erreur dans la lecture de la reponse");
	  			close(webSocket);
	  			exit(5);
			}else if(rd == 0)
			{//On regarde si le serveur a fermé la connexion
				//On ferme la socket web
				close(webSocket);
				FD_CLR(webSocket, &init_set);
				webSocket = -1;				
				printf("\n==================================\n");
				printf("La connexion avec le serveur web a été fermée\n");
				printf("\n==================================\n");
			}else
			{
				//On envoie la réponse au client
				printf("\n==================================\n");
				printf("Réponse envoyée : \n%s\n", response);
				printf("\n==================================\n");
				send(clientSocket, response, rd, 0);
			}
			nbfd--;
		}

		/*** on regarde si l'on a une requete du client ***/

		if(clientSocket >= 0 && FD_ISSET(clientSocket, &desc_set)){

			//On lit les données tant que le client en envoie. Une requête ne fait pas plus de 5000 octets.
			memset(requete, 0, MAXREQUEST);
			rd = recv(clientSocket, requete, MAXREQUEST, 0);

			//On vérifie qu'il n'y ait pas eu d'erreur dans la lecture
			if(rd < 0)
			{
				perror("Erreur dans la lecture de la requête");
	  			close(clientSocket);
	  			exit(5);
			}else if(rd == 0) 
			{ //On regarde si le client a fermé la connexion

				//On ferme la socket client
				close(clientSocket);
				FD_CLR(clientSocket, &init_set);
				clientSocket = -1;

				printf("\n==================================\n");
				printf("La connexion avec le client a été fermée\n");
				printf("\n==================================\n");

			}else
			{
				//On récupère le type de la requête
				searchTypeRequest(requete, type_requete);

				printf("\n==================================\n");
				printf("la requete est de type : %s\n", type_requete);

				//On ne considère que les requêtes GET
				if(strcmp(type_requete, "GET") == 0)
				{
					searchHostName(requete, hostname);
					printf("Hostname desire : %s\n", hostname);
					printf("Requete complete : \n%s\n", requete);

					//On crée la socket de dialogue avec le serveur web
					char service[] = "80";
					webSocket = createWebSocket(hostname, service);

					if(webSocket >= maxfd) 
					{
						maxfd = webSocket +1;
					}

					FD_SET(webSocket, &init_set);

					//Puis enfin on envoie la requête au serveur web
					send(webSocket, requete, rd, 0);

					//On met la requete dans un fichier de log
					char request[150];
					searchRequest(requete, request);
					addRequestLog(clientSocket, type_requete, request);
				}else if(strcmp(type_requete, "CLOSE"))
				{
					//On ferme la socket client
					close(clientSocket);
					FD_CLR(clientSocket, &init_set);
					clientSocket = -1;

					printf("\n==================================\n");
					printf("La connexion avec le client a été fermée\n");
					printf("\n==================================\n");
				}else if(strcmp(type_requete, "CONNECT"))
				{
						//On a du HTTPS
				}else{
					printf("Type de requete non taité : %s\n", type_requete);
				}

				printf("\n==================================\n");
			}
			nbfd--;
		}
	}

	close(serverSocketIPv4);
	close(serverSocketIPv6);

	return 0;
}