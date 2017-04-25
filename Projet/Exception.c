#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int contient(char * chaine, char * strArray[], int i)
{
    int resultat = 0;
	char * ret;
	for (int j=0;j<i;j++)
	{
		if((ret = strstr(chaine, strArray[j])) != NULL)
		{
			resultat++;
			chaine = ret;
		}
	}if (resultat == i)
	{
		return 1;
	}else
	{
		return 0;
	}
}

int beginWithDoubleBar(char* line) 
{
	if (*line == '|' && *(line+1) == '|') {
		return 1;
	}
	else return 0;
}


int starExprCase(char* chaine, char * line2) 
{
	char * token = NULL;
	int returnValue;	
	int k = 0;
	char * strArray[10];
	//Si la ligne contient des étoiles 
	if(strchr(line2, '*')!=NULL)
	{
		
		token = strtok(line2, "*");
		while( token != NULL) 
		{
			

			strArray[k] = malloc(strlen(token));
			strcpy(strArray[k], token);
			token = strtok(NULL, "*");
			k++;
		}
		free(line2);
		// On cherche si les éléments parsés se retrouvent dans chaine
		if (contient(chaine, strArray,k))
		{
			//printf("Motif reconnu\n");
			returnValue = 1; // On a trouvé ce motif dans chaine
		}else
		{
			returnValue = 0; // On a pas trouvé ce motif dans chaine
		}
		//free de notre tableau de string 
		for (int j=0;j<k;j++)
		{
			free(strArray[j]);
		}
		return returnValue;
	}
	else { // On a pas d'étoile est donc on considère l'expression
		strArray[0] = malloc(strlen(line2));
		strcpy(strArray[0], line2);
		if (contient(chaine, strArray,1))
		{
			//printf("Motif reconnu\n");
			returnValue = 1;	// On a trouvé ce motif dans chaine
		}else
		{
			returnValue = 0; //On a pas trouvé ce motif
		}
		free(strArray[0]);
		return returnValue;
	}
}

int isAnException(char* line) 
{
	if (*line == '@' && *(line+1) == '@') 
	{
		line = line + 2; //On fait deja le décalage
		return 1; //c'est une exceptions à la liste de rejet
	}
	return 0; //c'est une regle de rejet classique
}


int main(int argc, char **argv){
	FILE *infile;
	infile = fopen("MignonnesExceptions.txt", "r");
	char * line = NULL;
	char * line2 = NULL;
    char * chaine = "http://lel.com/quisoibonbonbonrhgveutsrbdesvsbonjourrchats/nope"; //La chaine dans laquelle on cherche à reconnaitre les motifs
    size_t len = 0;
    ssize_t read;
    char* httpCST = "http://";
    char* httpsCST = "https://";
    char* httpVAR; 
    char* httpsVAR;
    char* token;
    int isException = 0;
    int isRejet = 0;
    int foundIn = 0;
	if (infile == NULL) 
	{
		printf("Erreur à l'ouverture du fichier.\n"); 
	
	}else
	{
		printf("Fichier ouvert avec succès.\n");
		
		//On commence à lire le fichier d'exception.
		while(((read = getline(&line, &len, infile)) != -1))
		{
			isException = 0;
			foundIn = 0;
			line2 = calloc(1,strlen(line)-1);
			strncpy(line2, line, strlen(line)-1);
			if(strchr(line2, '$')!=NULL) 
			{ // Pour simplifier nous ignorerons les options qui sont dans une règle de rejet où d'exception de rejet, nous reviendrons plupart si le temps nous le permet sur la gestion des options
				strtok(line2, "$"); // Si on voit un '$', on met un '\0' pour bloquer la lecture de la suite, on ignore donc facilement les options
			}
			if (isAnException(line2)) 
			{
				line2 = line2 + 2*sizeof(char);
				isException = 1;
			}

			if (beginWithDoubleBar(line2)) 
			{
				httpVAR = calloc(1,strlen(httpCST)); //On construit les 2 char* contenant http:// et https:// au début de 'line2'
				strcat(httpVAR, httpCST);
				strcat(httpVAR, line2+2);
				*(httpVAR+strlen(httpVAR)) = '\0'; //On vire les '\n' parce que sinon on reconnaitra rien
				httpsVAR = calloc(1,strlen(httpsCST));
				strcat(httpsVAR, httpsCST);
				strcat(httpsVAR, line+2);
				*(httpsVAR+strlen(httpsVAR)-1) = '\0';
				//printf("%s\n", httpVAR);
				if (starExprCase(chaine, httpVAR))
				{
				 	foundIn = 1; //Si on trouve cette expression (starExprCase renvoie 1 si on reconnait #1 dans #0), alors on signal que le motif doit être rejeté
				 	isRejet = 1;
				} 
				if (starExprCase(chaine, httpsVAR))
				{
				 	foundIn = 1; //Si on trouve cette expression (starExprCase renvoie 1 si on reconnait #1 dans #0), alors on signal que le motif doit être rejeté
				 	isRejet = 1;
				} 
				//free(httpVAR);
				//free(httpsVAR);
			}

			else {
				if (starExprCase(chaine, line2))
				{
				 	foundIn = 1;
				 	isRejet = 1;
				} 
			}
			
			if (isException) 
			{
				if (isRejet) 
				{
					fclose(infile);
					printf("0\n");
					return 0; //On sait qu'on a une règle d'exception de rejet trouvée dans 'chaine', on a pas besoin de chercher plus loin
				}
				
			}
			
		}		
		fclose(infile);
	}
	printf("%d\n", isRejet);
	return isRejet; //Si on arrive ici c'est qu'on a pas trouvé d'exception de rejet, donc on renvoie si on a rencontré un rejet au moins une seule fois.
}


			
			//on ne lit pas les commentaires
				/*else{
					printf("Bloqué %s %s\n",line, chaine);				
					fclose(infile);
					free(line2);
					return 0;
			else if(strncmp(line,"@@",2)==0){
				printf("%s\n",line);
				}
				}*/		
