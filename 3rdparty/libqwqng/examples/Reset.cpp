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

    /* Reset Device */
    printf("\n%s\n\n", "Reset Device...");  
    if ( (qngStatus = QNG->Reset()) != S_OK) 
	{
		printf("Error: %s\n\n", QNG->StatusString()); 	
		return EXIT_FAILURE;
    }
    
    delete QNG;
        
    printf("\n\n%s\n\n", "EXIT..."); 

    return EXIT_SUCCESS;
}


