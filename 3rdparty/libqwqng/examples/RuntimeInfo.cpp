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
  
    /* Get Internal Stats for all RNG channels */
    printf("%s\n\n", "Get Internal Stats"); 			
    float* statsInfo;
    int infoCount;
    infoCount = 17;
    statsInfo = new float [infoCount];
    if ( (qngStatus = QNG->RuntimeInfo(statsInfo)) != S_OK) 
	{
		printf("Error: %s\n\n", QNG->StatusString()); 
		return EXIT_FAILURE;
    }
    for (int i=0; i<infoCount; i++) {
        printf("%02i: %1.9f\n", i, statsInfo[i]);  
    }  
    delete [] statsInfo;
 
    delete QNG;
        
    printf("\n\n%s\n\n", "EXIT..."); 

    return EXIT_SUCCESS;
}


