/* Simple program to demonstrate CC_USB in DAQ / stack mode
 * 
 * Copyright (C) 2005-2014 WIENER, Plein & Baus, Corp.
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation, version 2.
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
//setup output file
    FILE * DataFile;
    DataFile = fopen("CC_Data.txt","w");
    xxusb_device_type devices[100]; 
    struct usb_device *dev;
    usb_dev_handle *udev;       // Device Handle 
    long loops, LAM;
    long stack [100];
    int i,j,k,k_loops, error_count=0, events=0, event_size=0;
    //	unsigned int IntArray [10000];  //-> produces 32 bit data structure, use only with event alignment!
    short IntArray [10000];  //for FIFOREAD
    //	char IntArray [10000];  //for FIFOREAD
    //	char * ByteAP;
    long data_lines;
    long CamDummy;
    int ret,CamQ, CamX;
    int CAM_2249=3;			// station number for 2249 ADC
    
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

    //LeCroy 2249 DAQ test
    // prepare stack and load to CC-USB
    CamN=(CAM_2249<<9);
    stack[0] = 14;				// number of lines to follow
    for (i=0; i<=11; i++) stack[i+1] = CamN+(i<<5)+2;	// Read channels F(2) A(i)
    //	stack[13] = CamN+0x0009;		// CLEAR LAM N()with F(9) A(0) ==> not needed with F(2)A(11)
    stack[13] = 0x0010;				// add marker
    stack[14] = 0xffff;				// marker = ffff
    // Load stack into CC-USB
    ret = xxusb_stack_write(udev, 2, stack);
    // Define Trigger LAM=0 -> extern, otherwise LAM
    ret = CAMAC_write(udev, 25,9,16,LAM,&CamQ, &CamX);
    // Define Trigger Delay / LAM time out in Delay Register N(25) A(2) F(16) and enable /disable LAM
    if (LAM = 0)
                        {
                                //  Define Trigger Delay to 100us for external LAM; bits 0 to 15 in Delay Register N(25) A(2) F(16)
                                ret = CAMAC_write(udev, 25, 2, 16, 0x64,&CamQ, &CamX);
                                //  Prepare LeCroy ADC (disable LAM)
                                ret = CAMAC_read(udev, CAM_2249, 0, 24, &CamDummy,&CamQ, &CamX);
                        }
                        else
                        {
                                //  Define LAM time out to 100us for external LAM; bits 0 to 15 in Delay Register N(25) A(2) F(16)
                                ret = CAMAC_write(udev, 25, 2, 16, 0x640000,&CamQ, &CamX);
                                //  Prepare LeCroy ADC (enable LAM)
                                ret = CAMAC_read(udev, CAM_2249, 0, 26, &CamDummy,&CamQ, &CamX);
                        }
                // Set buffer size to 4k BuffOpt in Global Mode register N(25) A(1) F(16)
                        ret = CAMAC_write(udev, 25, 1, 16, 0x0,&CamQ, &CamX);

                // Prepare data file
                          fprintf(DataFile,"CC-USB test data file \n");
                // START DAQ
                //	printf("switch to DAQ & Reading data\n");
                //  Clear Data and LAM
                        ret = CAMAC_read(udev, CAM_2249, 0, 9, &CamDummy,&CamQ, &CamX);

                //  Start DAQ mode
                        ret = xxusb_register_write(udev, 1, 0x1); // start acquisition
                    k_loops=0;
                    while(k_loops<loops) // number of loops to read
                    {
                                ret = xxusb_bulk_read(udev, IntArray, 8192, 100);	// use for32-bit data
                                data_lines = ret / 2;
                                event_size = (IntArray[1] & 0xffff);
                                if (event_size > 0x100) event_size = 0x100;
                                if (DataFile !=NULL)
                                {

                                        if(ret>0)
                                        {
                                                events = (IntArray[0]& 0xffff);
                //				printf("Events in loop %i : %i\n ",k_loops, events);
                                                for (j=0; j<=data_lines; j++) fprintf(DataFile,"%hx\t",IntArray[j]);
                                        }
                                        else
                                        {
                                                error_count ++;
                                                printf("no read\n");
                                        }
                                         fprintf(DataFile,"\n");
                                }
                                k_loops ++;
                        }
                // leave DAQ mode
                    xxusb_register_write(udev, 1, 0x0);
                // drain all data from fifo
                        ret=1;
                        k=0;
                        while(ret>0)
                    {
                                ret = xxusb_bulk_read(udev, IntArray, 8192, 100);
                                if(ret>0)
                                {
                //			printf("drain loops: %i (result %i)\n ",k,ret);
                                        k++;
                                        if (k>100) ret=0;
                                }
                        }
                //	in case still DAQ mode -> leave!!!
                //  xxusb_register_write(hdev, 1, 0x0);
                        fclose(DataFile);
                        return 0;


    // Close the Device
    xxusb_device_close(udev);
    printf("\n\n\n");
    
    return 0;
}

