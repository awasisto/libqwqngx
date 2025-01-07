#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qwqng.hpp>

QWQNG* QNG;

int main(void)
{
    int qngStatus = 0;
    
    QNG = QWQNG::Instance(); // create single instance of class
     
    printf("\n%s\n\n", "Start Device...");   

    /* Print Status String */ 
    char* statusString; 
    statusString = QNG->StatusString();
    printf("Status: %s\n\n", statusString);             	
    
    /* Print open device serial number */
    char* SerialNumber; 
    SerialNumber = QNG->DeviceID();
    printf("DeviceID %s\n\n", SerialNumber);

    /* Get random uniform number [0,1) as double */
    printf("\n%s\n\n", "Get random uniform number");    
    double randuniform;
    if ( (qngStatus = QNG->RandUniform(&randuniform)) != S_OK) 
	{
		printf("Error: %s\n\n", QNG->StatusString()); 
		return EXIT_FAILURE;
    }
    printf("%f\n", randuniform);	  
    
    delete QNG;
        
    printf("\n\n%s\n\n", "EXIT..."); 

    return EXIT_SUCCESS;
}


