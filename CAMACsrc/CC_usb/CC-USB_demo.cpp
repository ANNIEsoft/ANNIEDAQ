/* Simple program to demonstrate CC-USB
 * 
 * Copyright (C) 2005-2014 WIENER, Plein & Baus, Corp
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation, version 2.
 *
 *
 *    
 *

*/

#include <libxxusb.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <cstdlib>
#include <string.h>


int main (int argc,  char *argv[])
{

    int CamN, CamA, CamF;
    long CamD;
    int CamQ, CamX;
    char nafin[20];
    char nafinx[20];
    int WriteMode;

    int ret, i;
    xxusb_device_type devices[100]; 
    struct usb_device *dev;
    usb_dev_handle *udev;       // Device Handle 
    
    
    //Find XX_USB devices and open the first one found
    xxusb_devices_find(devices);
    dev = devices[0].usbdev;
    udev = xxusb_device_open(dev); 
    
    // Make sure CC_USB opened OK
    if(!udev) 
    {
	     printf ("\n\nFailedto Open CC_USB \n\n");
	     return 0;
    }
    
    
    
    //Make some lights dance
    printf("\n\n\nTesting data lines....");

    CAMAC_Z(udev);
		CAMAC_I(udev, true);
		CAMAC_write(udev, 1, 0, 16, 0xaaaaaa,&CamQ, &CamX);
		CAMAC_read(udev, 1, 0, 0, &CamD,&CamQ, &CamX);
		CAMAC_C(udev);
		CAMAC_I(udev, false);
		CAMAC_Z(udev);
	  for (i=0; (i <= 100000); i++)
			CAMAC_write(udev, 1, 0, 16, i,&CamQ, &CamX);
		printf(" Done.\n\n");
    
    
    
    
    strcpy(nafin,"5,5,0");
		sscanf(nafin,"%i,%i,%i",&CamN,&CamA,&CamF);
		while (CamN>0)
		{
			printf("    N,A,F (Comma-separated; x for exit; p for NAF=%i,%i,%i) -> ",CamN, CamA, CamF);
			fflush(stdin);
			scanf("%s",&nafinx);
			if (nafinx[0]=='X' || nafinx[0]=='x') 
			{
				break;
			}
			if (strlen(nafinx)>4)
				strcpy(&nafin[0], &nafinx[0]);
			sscanf(nafin,"%i,%i,%i",&CamN,&CamA,&CamF);
			fflush(stdin);
			if (CamF < 8)	
			{
				ret = CAMAC_read(udev, CamN, CamA, CamF, &CamD,&CamQ, &CamX);
				if (ret < 0)
					printf("Read Operation Failed\n");
				else
					printf("\n       X = %i, Q = %i, D = %lx\n\n",CamX, CamQ, CamD);
			}
			if ((CamF > 7) && (CamF < 16))
			{
				ret = CAMAC_read(udev, CamN, CamA, CamF, &CamD,&CamQ, &CamX);
				if (ret < 0)
					printf("Write Operation Failed\n");
				else
					printf("\n       X = %i, Q = %i\n\n",CamX, CamQ);
			}
			if ((CamF > 15) && (CamF < 24))
			{	
				WriteMode=1;
				printf("     D (Use 0x Prefix for Hexadecimal)-> ");
				scanf("%li", &CamD);
				fflush(stdin);
				CAMAC_write(udev, CamN, CamA, CamF, CamD,&CamQ, &CamX);
			}	
		}
    
    
    
  


    // Close the Device
    xxusb_device_close(udev);
    printf("\n\n\n");
    
    return 0;
}

