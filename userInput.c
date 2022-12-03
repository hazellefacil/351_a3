// 1. Trim whitespace from the end of the input string.

// 2. Display how many characters are in the input after trimming, and print the string of
// text to the screen.

// 3. Encode the message into Morse code and display the encoded Morse code message to
// the screen.

// ▪ For each dot-time, print out an ‘X’ if the light will be on, or print out a ‘_’ if the
// LED will be off.

// 4. Send message or encoded Morse code to the PRU to flash out (see below).

// 5. Wait until the PRU is finished flashing, then prompt the user for another line of text

#include "userInput.h"
#include <math.h>
#include <stdio.h>
#include <unistd.h>



static unsigned short morsecode_codes[] = {
		0xB800,	// A 1011 1
		0xEA80,	// B 1110 1010 1
		0xEBA0,	// C 1110 1011 101
		0xEA00,	// D 1110 101
		0x8000,	// E 1
		0xAE80,	// F 1010 1110 1
		0xEE80,	// G 1110 1110 1
		0xAA00,	// H 1010 101
		0xA000,	// I 101
		0xBBB8,	// J 1011 1011 1011 1
		0xEB80,	// K 1110 1011 1
		0xBA80,	// L 1011 1010 1
		0xEE00,	// M 1110 111
		0xE800,	// N 1110 1
		0xEEE0,	// O 1110 1110 111
		0xBBA0,	// P 1011 1011 101
		0xEEB8,	// Q 1110 1110 1011 1
		0xBA00,	// R 1011 101
		0xA800,	// S 1010 1
		0xE000,	// T 111
		0xAE00,	// U 1010 111
		0xAB80,	// V 1010 1011 1
		0xBB80,	// W 1011 1011 1
		0xEAE0,	// X 1110 1010 111
		0xEBB8,	// Y 1110 1011 1011 1
		0xEEA0	// Z 1110 1110 101
};




int main(void)
{
    getUserInput();
    return 0;
}

void getUserInput()
{
    char *buff = NULL;
    size_t sizeAllocated = 0;

    printf("Hello!! Beginning Morse Code!! \n");
    printf("> ");
    size_t numCh = getline(&buff, &sizeAllocated, stdin);

    // use strcspn to remove the enter key 
    buff[strcspn(buff, "\n")] = 0;

    // enter key is pressed + no characters entered 
    if(buff[0] == 0)
    {
        printf("Shutting down application! Goodbye :) \n");
        exit(1);
    }

    fprintf(stderr, "Flashing out %zu characters: '%s' \n",numCh, buff);
    fflush(stderr);

    encodeMessage(buff, numCh);

    free (buff);
    buff = NULL;

}

void encodeMessage(char * buffer, size_t numCh)
{
        // from the character, find the corresponding letter in the struct
        unsigned short *encodedMsg = NULL;
        encodedMsg = (unsigned short*) malloc(numCh * sizeof(unsigned short));

        //store the binary version of the string in an array 
        long long int *binMsg = NULL;
        binMsg = (long long int*) malloc(numCh * sizeof(long long int));
        
        for(int i = 0; i < numCh; i++)
        {
            encodedMsg[i] = findLetter(buffer[i]);

            long long int bin = encodedToBin(encodedMsg[i]); 
            binMsg[i] = bin;
        }

        binToMorse(binMsg, numCh);

}

unsigned short findLetter(char c)
{
    // perform bitwise operation wtih 31 to give the letter's position 
    const int andNum = 31;
    int position = c & andNum; 

    //indexing begins at zero, subtract 1 
    position = position - 1; 

    unsigned short encodedLetter = morsecode_codes[position];
    return encodedLetter;
}


long long int encodedToBin(unsigned short encodedLetter)
{
    long long int binNumber = 0;
    int power = 0;
    int encodedLet = (int) encodedLetter;

    while(encodedLet > 0)
    {
        int remainder = encodedLet % 2; 
        long long int temp = pow(10,power); 
        binNumber += remainder * temp; 
        power ++;
        encodedLet /= 2;
    }
    return binNumber;
}




//before next letter, print three dashes 
// for the PRU to flash the LED 
void binToMorse(long long int *binNum, size_t numCh)
{

    int longIntDec = 16;
    for(int i = 0; i < numCh; i++)
    {

        long long int num = binNum[i];

        printCleanMorse(num);

        if(i < numCh -2){
            fprintf(stderr,"___");
            fflush(stderr);
            sleep(1);
            
        }
    }
        printf("\n");
}


void printCleanMorse(long long int binNum)
{
    int longIntDec = 16;
    int count = 0;
    int validDecPlaces = 0;
    long long int cpBinNum = binNum;

    while(count< longIntDec)
    {
        int mod = binNum%10;
        if(mod == 1)
        {
            validDecPlaces = longIntDec - count;
            break;
        }
        binNum = binNum/10;
        count++;
    }
   

    int count2 = 0;
    int invZeroes = longIntDec - validDecPlaces;
    //printf("invalid zeroes: %d", invZeroes);

        for(int i =0 ; i < invZeroes; i++){
            int check = cpBinNum % 10;
            cpBinNum = cpBinNum / 10;
        }
    while(count2 < validDecPlaces)
    {
 
        //ignore all of the invalid zeroes 
        int check = cpBinNum % 10;

            if(check == 1)
            {
                fprintf(stderr, "X");
                fflush(stderr);
                sleep(1);
            
            }
            if(check == 0)
            {
                fprintf(stderr,"_");
                fflush(stderr);
                sleep(1);
            }
        
        cpBinNum = cpBinNum / 10;
        count2++;
    }

}


