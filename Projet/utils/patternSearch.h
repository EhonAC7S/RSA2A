#ifndef PATTERNSEARCH
#define PATTERNSEARCH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int contient(char * chaine, char * strArray[], int i);
int beginWithDoubleBar(char* line);
int starExprCase(char* chaine, char * line2);
int isAnException(char* line);
int searchPatternIn(char* chaine);

#endif