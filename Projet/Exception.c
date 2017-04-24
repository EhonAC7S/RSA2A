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

int beginWithDoubleBar(char* line) {
	if (*line == '|' && *(line+1) == '|') {
		return 1;
	}
	else return 0;
}


int starExprCase(char* chaine, char * line2) {
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
			printf("Motif reconnu\n");
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
			//printf("à reconnaitre  : %s\n", line2);
			printf("Motif reconnu\n");
			returnValue = 1;	// On a trouvé ce motif dans chaine
		}else
		{
			returnValue = 0; //On a pas trouvé ce motif
		}
		free(strArray[0]);
		return returnValue;
	}
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

	if (infile == NULL) 
	{
		printf("Erreur à l'ouverture du fichier.\n"); 
	
	}else
	{
		printf("Fichier ouvert avec succès.\n");
		
		//On commence à lire le fichier d'exception.
		while(((read = getline(&line, &len, infile)) != -1))
		{
			line2 = calloc(1,strlen(line)-1);
			strncpy(line2, line, strlen(line)-1);
			if(strchr(line2, '$')!=NULL) 
			{ // Pour simplifier nous ignorerons les options qui sont dans une règle de rejet où d'exception de rejet, nous reviendrons plupart si le temps nous le permet sur la gestion des options
				strtok(line2, "$"); // Si on voit un '$', on met un '\0' pour bloquer la lecture de la suite, on ignore donc facilement les options
			}


			if (beginWithDoubleBar(line)) {
				
				httpVAR = calloc(1,strlen(httpCST)); //line dont on retire '/n' a la fin et +7 pour "http://" donc +6
				strcat(httpVAR, httpCST);
				strcat(httpVAR, line+2);
				*(httpVAR+strlen(httpVAR)-1) = '\0';
				httpsVAR = calloc(1,strlen(httpsCST));
				strcat(httpsVAR, httpsCST);
				strcat(httpsVAR, line+2);
				*(httpsVAR+strlen(httpsVAR)-1) = '\0';
				starExprCase(chaine, httpVAR); 
				starExprCase(chaine, httpsVAR);
				//free(httpVAR);
				//free(httpsVAR);
			}

			else {
				starExprCase(chaine, line2);
			}
			
		}		
		fclose(infile);
	}
	return 0;
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
