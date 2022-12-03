#ifndef INP_HEADER
#define INP_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void getUserInput();
void encodeMessage(char * buffer, size_t numCh);
static unsigned short findLetter(char c);
long long int encodedToBin(unsigned short encodedLetter);
void binToMorse(long long int *binNum, size_t numCh);
void printCleanMorse(long long int binNum);

#endif
