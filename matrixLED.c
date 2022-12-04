// PRU no message, LED should be blank

// each column reps one dot time 
// left most represent current dot time 


// left most represents dash 
// 

// read the isLedOn and grab whether it's a dot or dash


#include "matrixLED.h"
#include <string.h>

static int numRows = 8;
static int left80 = 0b10000000;
static int left01 = 0b00000001;
static int left02 = 0b00000010;
static int left04 = 0b00000100;

static int right08 = 0b00001000;
static int right10 = 0b00010000;
static int right20 = 0b00100000;
static int right40 = 0b01000000;


int morseToDisp[8];

int main(void)
{

    int testingArr[] = {0,1,0,1,0,1,0,1,0,1,1,1,1,1,1,1};

    int size = sizeof(testingArr) / sizeof(int);

    printf("size of arr %d \n", size);

    printf("printing X0X0X0X00\n");
    printMorseRT(testingArr);
    //X_X_X_X___X___X_X_XXX_X___X_X_^C

    return 0;
}

void printMorseRT(int * binArr)
{

    int sizeOfArr = 16;

    int position = 7;

    int cpBinArr[numRows];

    //initialize the first thing to display
    for(int i = 0; i < numRows; i++)
    {
        cpBinArr[i] = binArr[i];
    }

    while(position <= sizeOfArr)
    {
        //fill the matrix with the morse code 
        for(int i = 0; i < numRows; i++)
        {
            morseToDisp[i] = cpBinArr[i];
        }
    
        display();
        sleep(3);

        //update the matrix for the next display
        for(int i = 0; i < numRows - 1; i++)
        {
            cpBinArr[i] = cpBinArr[i+1];
        }
        //move the position over 
        position++;
        cpBinArr[numRows - 1] = binArr[position];
    }

}


int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr,int values[])
{
    unsigned char buff[17];
    buff[0] = regAddr;

    for(int i = 1; i < 18; i++){
        buff[i] = values[i-1];
    }

    int res = write(i2cFileDesc, buff, 17);

    if (res != 17) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

int * leftDigit()
{

    int *leftBin;
    leftBin = (int*)malloc(numRows * sizeof(int) * 5);

    int binHex80 = 0, binHex02 = 0, binHex01 = 0, binHex04 = 0;

    if(morseToDisp[0] == 1)
    {
        binHex80 = left80;
    }

    if(morseToDisp[1] == 1)
    {
        binHex01  = left01;
    }

    if(morseToDisp[2] == 1)
    {
        binHex02 = left02;
    }

    if(morseToDisp[3] == 1)
    {
        binHex04 = left04;
    }


    for(int i = 0; i < numRows;i++){

        leftBin[i] = binHex80 + binHex01 + binHex02 + binHex04;
    }

    return leftBin;
}

int * rightDigit(){

    int *rightBin;
    rightBin = (int*)malloc(numRows * sizeof(int) * 5);


    int binHex08 = 0, binHex10 = 0, binHex20 = 0, binHex40 = 0;

    if(morseToDisp[4] == 1)
    {
        binHex08 = right08;
    }

    if(morseToDisp[5] == 1)
    {
        binHex10 = right10;
    }

    if(morseToDisp[6] == 1)
    {
        binHex20 = right20;
    }

    if(morseToDisp[7] == 1)
    {
        binHex40  = right40;
    }


    for(int i = 0; i < numRows;i++){

        rightBin[i] = binHex08 + binHex10 + binHex20 + binHex40;

    }

    return rightBin;
}


void display()
{

    int *left;
    left = (int*)malloc(numRows * sizeof(int) * 5);
    left = leftDigit();
    

    int *right;
    right = (int*)malloc(numRows * sizeof(int) * 5);
    right = rightDigit();


    int *toDisplay;
    toDisplay= (int*)malloc(numRows * sizeof(int) * 5);

    for(int i = 0 ; i< numRows;i++){
        toDisplay[i] = left[i] | right[i];
    }


    free(left);
    free(right);

   int valueArray[17];
    for(int i=0; i < 18; i++){
        valueArray[i] = 0x00;
    }

    for(int i=0; i<7; i++){
        valueArray[i*2] = toDisplay[i];
    }
    free(toDisplay);

   int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, REG_DIRA, valueArray);
    close(i2cFileDesc);

}



