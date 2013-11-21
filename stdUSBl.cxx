/*****
 
stdUSBL - A libusb implementation of ezusbsys.

StdUSB libusb implementation used here uses same function interface with native stdUSB Windows WDM driver.
 
*****/



#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "stdUSB.h"

stdUSB::stdUSB(void){ 
   //printf("stdUSB::stdUSB()\n");
   for(int i=0;i<MAX_STDUSB_DEVICES;i++)
      stdUSB::stdHandle[i] = INVALID_HANDLE_VALUE;
   numHandles=0;
   //   libusb_set_debug();
   //    createHandles();
}

stdUSB::~stdUSB(void) {
   //printf("stdUSB::~stdUSB()\n");
   for(int i=0;i<MAX_STDUSB_DEVICES;i++)
      if(stdHandle[i] != INVALID_HANDLE_VALUE) {
	 freeHandle(i);
	 numHandles--;
      }
}

/**
 * Finds USBFX2 device, opens it, sets configuration, and claims interface.
 *  Using of goto is mad bad. Check out linux kernel sources.
 * @param  
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::createHandles(int *busNums, int *devNums, int expNum) {
  //printf("stdUSB::createHandles()\n");
  
  int requestDevice=0;
  int count=countDevices();
  if(expNum==count) {
    requestDevice=1;
  }

    
  //int count=1;
  int retval=0;
  if(count>0) {
    for(int usbDevInd=0;usbDevInd<count;usbDevInd++) {
      if(requestDevice)
	retval+=createHandle(usbDevInd,busNums[usbDevInd],devNums[usbDevInd]);
      else {
	retval+=createHandle(usbDevInd);//,busNums[usbDevInd],devNums[usbDevInd]);
      }
    }
  }
  if(numHandles!=count) {
    printf("Expected %d handles got %d\n",count,numHandles);
    return FAILED;
  }
  return SUCCEED;
}

/**
 * Finds USBFX2 device, opens it, sets configuration, and claims interface.
 *  Using of goto is mad bad. Check out linux kernel sources.
 * @param  
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::createHandle(int usbDevInd, int busNum, int devNum) {
  //printf("stdUSB::createHandle(%d)\n",usbDevInd);
    int retval;
    struct usb_device *dev;
   
    if (stdHandle[usbDevInd] != INVALID_HANDLE_VALUE)
       goto ok;
    
    dev = stdUSB::init(usbDevInd,busNum,devNum);
    retval = (long) dev;

    if (retval == 0) {
      printf("stdUSB::init() %d %s\n",retval,strerror(-1*retval));
      goto fail;
    }
    //printf("stdHandle[%d] = %d\t dev = %d\n",usbDevInd,stdHandle[usbDevInd],dev);
    stdHandle[usbDevInd] = usb_open(dev);
    //printf("stdHandle[%d] = %d\t dev = %d\n",usbDevInd,stdHandle[usbDevInd],dev);
    if (stdHandle[usbDevInd] == INVALID_HANDLE_VALUE) {
        printf("usb_open(dev) fail\n");
        goto fail;
    }

    retval = usb_set_configuration(stdHandle[usbDevInd], USBFX2_CNFNO);
    if (retval != 0) {
      printf("usb_set_configuration -- fail %d %s\n",retval,strerror(-1*retval));
      goto fail;
    }

    retval = usb_claim_interface(stdHandle[usbDevInd], USBFX2_INTFNO);
    if (retval != 0) {
      printf("usb_set_configuration -- fail %d %s\n",retval,strerror(-1*retval));
        goto fail;
    }

    retval = usb_set_altinterface(stdHandle[usbDevInd], USBFX2_INTFNO);
    if (retval != 0) {
      printf("usb_set_altinterface -- fail %d %s\n",retval,strerror(-1*retval));
      goto fail;
    }

    goto ok;

    /* on ok */
ok:
    printf("createhandle: OK\n");
    numHandles++;
    
    return SUCCEED;

    /* on failure*/
fail:
    printf("createhandle: FAILED\n");
    return FAILED; // Unable to open usb device. No handle.}
}

/**
 * Internal function.
 *  Initialises libusb and finds the correct usb device.
 * @param  
 * @return usb_device* -- A pointer to USBFX2 libusb dev entry, or INVALID_HANDLE_VALUE on failure.
 */
struct usb_device* stdUSB::init(int usbDevInd, int busNum, int devNum) {
   //printf("stdUSB::init()\n");
    struct usb_bus *usb_bus;
    struct usb_device *dev;

    /* init libusb*/
    usb_init();
    usb_find_busses();
    usb_find_devices();
    int numFound=0;
    char testLabel[12];
    if(busNum>=0 && devNum>=0) {
      sprintf(testLabel,"%03d/%03d",busNum,devNum);
    }

    /* usb_busses is a linked list which after above init function calls contains every single usb device in the computer.
        We need to browse that linked list and find EZ USB-FX2 by VENDOR and PRODUCT ID */
    for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices; dev; dev = dev->next) {
            if ((dev->descriptor.idVendor == USBFX2_VENDOR_ID) && (dev->descriptor.idProduct == USBFX2_PRODUCT_ID)) {
	     
	      
	      char devLabel[12];
	      sprintf(devLabel,"%s/%s",dev->bus->dirname, dev->filename);
      
	      
	      
	      if(busNum>=0 && devNum>=0) {
		if(strncmp(devLabel,testLabel,7)==0) {
		  printf("init: found device: %s/%s\n",dev->bus->dirname, dev->filename);
		  return dev;		  
		}
		//		printf("testLabel %s devLabel %s strncmp %d\n",testLabel,devLabel,);
	      }
	      else if(usbDevInd==numFound) {
		printf("init: found device: %s/%s\n",dev->bus->dirname, dev->filename);
		return dev;
	       }
	       numFound++;
            }
        }
    }

    printf("init: device not found)");
    /* on failure (device not found) */
    return INVALID_HANDLE_VALUE;
}

/**
 * Internal function.
 *  Initialises libusb and finds the correct usb device.
 * @param  
 * @return usb_device* -- A pointer to USBFX2 libusb dev entry, or INVALID_HANDLE_VALUE on failure.
 */
int stdUSB::countDevices(void) {
   //printf("stdUSB::countDevices()\n");
    struct usb_bus *usb_bus;
    struct usb_device *dev;
    
    /* init libusb*/
    usb_init();
    usb_find_busses();
    usb_find_devices();

    int numFound=0;
    /* usb_busses is a linked list which after above init function calls contains every single usb device in the computer.
        We need to browse that linked list and find EZ USB-FX2 by VENDOR and PRODUCT ID */
    for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices; dev; dev = dev->next) {
            if ((dev->descriptor.idVendor == USBFX2_VENDOR_ID) && (dev->descriptor.idProduct == USBFX2_PRODUCT_ID)) {
	       printf("countDevices: found device: %ld\n", (long)dev);
	       numFound++;
            }
        }
    }

    printf("countDevices: Found %d devices\n",numFound);
    /* on failure (device not found) */
    return numFound;
}


/**
 * Frees handles and resources allcated by createHandle().
 * @param  
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::freeHandle(int usbDevInd) //throw(...)
{
   //printf("stdUSB::freeHandle()\n");
    /* release interface */
    int retval = usb_release_interface(stdHandle[usbDevInd], USBFX2_INTFNO);
    if (retval != 0)
        return FAILED;

    /* close usb handle */
    retval = usb_close(stdHandle[usbDevInd]);
    if (retval != 0)
        return FAILED;

    /* all ok */
    return SUCCEED;
}

bool stdUSB::freeHandles(void) {
   //printf("stdUSB::freeHandles()\n");
    // this function exits just because there is no reason
    // to open and close handle all time
    return SUCCEED;
}

/**
 * Frees handles and resources allcated by createHandle().
 * @param data -- command, control code, value, or such to be sent to the device
 * @return bool -- SUCCEED or FAILED
 */
 bool stdUSB::sendData(unsigned int data, int whichInd)// throw(...)
{
   //  printf("stdUSB::sendData(%u,%d)\n",data,whichInd);
    /* Shifted right because send value needs to be in HEX base. char[4] ^= int (char -> 1byte, int -> 4 bytes)*/
    char buff[4];
    buff[0] = data;
    buff[1] = (data >> 8);
    buff[2] = (data >> 16);
    buff[3] = (data >> 24);

    /*  USBFX2_EP_WRITE => end point addr.
    buff => bytes to send
    USB_TOUT_MS => timeout in ms
    */
    if(whichInd>0 && whichInd<=4) {
       int retVal = usb_bulk_write(stdHandle[whichInd-1], USBFX2_EP_WRITE, buff, sizeof(buff), USB_TOUT_MS);       
       //    printf("SendData: sent %d bytes\n", retVal);
       
       if (retVal == 4) //return value must be exact as the bytes transferred
	  return SUCCEED;
       else {
	  printf("Error sending data: %s\n",strerror(errno));
	  return FAILED;
       }
    }
    else if(whichInd==0) {
       int retVal=0;
       for(int usbDevInd=0;usbDevInd<numHandles;usbDevInd++) {
	  //	 printf("Trying to write to %d\n",stdHandle[usbDevInd]);
	  int tempRet= usb_bulk_write(stdHandle[usbDevInd], USBFX2_EP_WRITE, buff, sizeof(buff), USB_TOUT_MS);     
	  if(tempRet!=4) {
	    printf("Error sending data %d (nbytes %d): %s\n",usbDevInd,sizeof(buff),strerror(errno));
	  }
	  retVal += tempRet;
	  
       }	  
       if(retVal == numHandles*4)
	  return SUCCEED;
       else {
	  return FAILED;
       }
    }
    return FAILED;
}


/**
 * Performs bulk transfer to IN end point. Reads data off all boards.
 * @param pData -- pointer to input buffer data array
 * @param l -- size of the data buffer (think: pData[l] )
 * @param lread -- out param: number of samples read
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::readData(unsigned short * pData, int l, int* lread)// throw(...)
{
  //  printf("stdUSB::readData()\n");
   int retVal=0;
   int numRead[MAX_STDUSB_DEVICES];
   //RJN hack to fix xDONE issues
   int readOrder[4]={3,2,1,0};
   for(int tempInd=0;tempInd<numHandles;tempInd++) {
     int usbDevInd=readOrder[tempInd];
     retVal+=readData(&pData[usbDevInd*l],l,&numRead[usbDevInd],usbDevInd);
      
   }
   *lread=numRead[0];
   if(retVal==numHandles)
      return SUCCEED;
   return FAILED;      
}


/**
 * Performs bulk transfer to IN end point. Reads data off the board.
 * @param pData -- pointer to input buffer data array
 * @param l -- size of the data buffer (think: pData[l] )
 * @param lread -- out param: number of samples read
 * @return bool -- SUCCEED or FAILED
 */
    bool stdUSB::readData(unsigned short * pData, int l, int* lread, int usbDevInd)// throw(...)
{
    int buff_sz = l*sizeof(unsigned short);
    //char* buff = new char[110];
    //buff_sz = 110;

    /*  USBFX2_EP_READ => end point addr.
    buff => bytes to read (has to be <64 bytes)
    USB_TOUT_MS => timeout in ms
    */
    int retVal = usb_bulk_read(stdHandle[usbDevInd], USBFX2_EP_READ, (char*)pData, buff_sz, USB_TOUT_MS);

//    printf("readData: read %d bytes\n", retVal);

    if (retVal > 0) {
        //*lread = retVal/buff_sz;
        *lread = (int)(retVal / (unsigned long)sizeof(unsigned short));
        //*lread *= 4;
        return SUCCEED;
    } else {
      if(retVal==-110) { 
	//Think this is a timeout
	usleep(1000);
	retVal = usb_bulk_read(stdHandle[usbDevInd], USBFX2_EP_READ, (char*)pData, buff_sz, USB_TOUT_MS);
	if(retVal>0) {
	  *lread = (int)(retVal / (unsigned long)sizeof(unsigned short));
	  return SUCCEED;
	}
      }
      else {           
	printf("error code: %d %s\n", retVal,strerror(-1 * retVal));	  
	*lread = retVal;
      }
      return FAILED;
    }
}
