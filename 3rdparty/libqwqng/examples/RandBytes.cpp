#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qwqng.hpp>

QWQNG* QNG;

int main(void)
{
    int qngStatus = 0;
    
    QNG = new QWQNG(); // create class
     
    printf("\n%s\n\n", "Start Device..."); 

    /* Print Status String */ 
    char* statusString; 
    statusString = QNG->StatusString();
    printf("Status: %s\n\n", statusString);             	
    
    /* Print open device serial number */
    char* SerialNumber; 
    SerialNumber = QNG->DeviceID();
    printf("DeviceID %s\n\n", SerialNumber);

    /* Get 10 random bytes */
    printf("\n%s\n\n", "Get 10 Random Bytes (hex)"); 
    char* randbyte;	
    int bytecount;	
    bytecount = 10;
    randbyte = new char [bytecount];
    if ( (qngStatus = QNG->RandBytes(randbyte, bytecount)) != S_OK) 
	{
		printf("Error: %s\n\n", QNG->StatusString());
		return EXIT_FAILURE;
    }
    for (int i=0; i<bytecount; i++) 
	{
        printf("%02X ", randbyte[i]&0xFF); 
    } 
    delete [] randbyte;
    
    delete QNG;
        
    printf("\n\n%s\n\n", "EXIT..."); 

    return EXIT_SUCCESS;
}


