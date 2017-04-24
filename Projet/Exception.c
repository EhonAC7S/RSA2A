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


int starExprCase(char* chaine, char * line2, char * strArray[]) {
	char * token = NULL;
	int returnValue;	
	int k = 0;
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
	char * strArray[10];
    char * chaine = "http://lel.com/quisoirhgveutsrbdesvsrchats"; //La chaine dans laquelle on cherche à reconnaitre les motifs
    size_t len = 0;
    ssize_t read;
    char* httpCST = "http://";
    char* httpsCST = "https://";
    char* httpVAR; 
    char* httpsVAR;

	if (infile == NULL) 
	{
		printf("Erreur à l'ouverture du fichier.\n"); 
	
	}else
	{
		printf("Fichier ouvert avec succès.\n");
		
		//On commence à lire le fichier d'exception.
		while(((read = getline(&line, &len, infile)) != -1))
		{
			if (beginWithDoubleBar(line)) {
				
				httpVAR = calloc(1,strlen(line)+6); //line dont on retire '/n' a la fin et +7 pour "http://" donc +6
				strcat(httpVAR, httpCST);
				strcat(httpVAR, line+2);
				printf("%s\n", httpVAR);
				httpsVAR = calloc(1,strlen(line)+7);
				strcat(httpsVAR, httpsCST);
				strcat(httpsVAR, line+2);
				starExprCase(chaine, httpVAR,strArray); 
				starExprCase(chaine, httpsVAR,strArray);
			}

			else {
				line2 = calloc(1,strlen(line)-1);
				strncpy(line2, line, strlen(line)-1);
				starExprCase(chaine, line2, strArray);
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
