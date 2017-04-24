#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	FILE *infile;
	FILE* outfile;
	infile = fopen("Exception.txt", "r");
	outfile = fopen("rejectListModified.txt","w+");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	if (infile == NULL || outfile==NULL) 
	{
		printf("Unable to open file.\n"); 
	}else
	{
		printf("Fichier ouvert avec succès.\n");
		while((read = getline(&line, &len, infile)) != -1){
			if (*line != '!') 
			{
				fputs(line,outfile);
			}
		}		
		fclose(infile);
		fclose(outfile);
	}
	return 0;
}
