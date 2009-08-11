/*****
 
stdUSBL - A libusb implementation of ezusbsys.

StdUSB libusb implementation used here uses same function interface with native stdUSB Windows WDM driver.
 
*****/

#include <string.h>
#include <stdio.h>
#include "stdUSB.h"

stdUSB::stdUSB(void) {
    stdUSB::stdHandle = INVALID_HANDLE_VALUE;
    createHandles();
}

stdUSB::~stdUSB(void) {
    if(stdHandle != INVALID_HANDLE_VALUE)
        freeHandle();
}

/**
 * Finds USBFX2 device, opens it, sets configuration, and claims interface.
 *  Using of goto is mad bad. Check out linux kernel sources.
 * @param  
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::createHandles(void) {
    int retval;
    struct usb_device *dev;
   
    if (stdHandle != INVALID_HANDLE_VALUE)
        goto ok;
    
    dev = stdUSB::init();
    retval = (long) dev;

    if (retval == 0)
        goto fail;

    stdHandle = usb_open(dev);
    if (stdHandle == INVALID_HANDLE_VALUE)
        goto fail;

    retval = usb_set_configuration(stdHandle, USBFX2_CNFNO);
    if (retval != 0)
        goto fail;

    retval = usb_claim_interface(stdHandle, USBFX2_INTFNO);
    if (retval != 0)
        goto fail;

    retval = usb_set_altinterface(stdHandle, USBFX2_INTFNO);
    if (retval != 0)
        goto fail;

    goto ok;

    /* on ok */
ok:
//printf("createhandles: OK\n");
    return SUCCEED;

    /* on failure*/
fail:
printf("createhandles: FAILED\n");
    return FAILED; // Unable to open usb device. No handle.}
}

/**
 * Internal function.
 *  Initialises libusb and finds the correct usb device.
 * @param  
 * @return usb_device* -- A pointer to USBFX2 libusb dev entry, or INVALID_HANDLE_VALUE on failure.
 */
struct usb_device* stdUSB::init(void) {
    struct usb_bus *usb_bus;
    struct usb_device *dev;

    /* init libusb*/
    usb_init();
    usb_find_busses();
    usb_find_devices();

    /* usb_busses is a linked list which after above init function calls contains every single usb device in the computer.
        We need to browse that linked list and find EZ USB-FX2 by VENDOR and PRODUCT ID */
    for (usb_bus = usb_busses; usb_bus; usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices; dev; dev = dev->next) {
            if ((dev->descriptor.idVendor == USBFX2_VENDOR_ID) && (dev->descriptor.idProduct == USBFX2_PRODUCT_ID)) {
//                printf("init: found device: %d\n", (int)dev);
                return dev;
            }
        }
    }

    printf("init: device not found)");
    /* on failure (device not found) */
    return INVALID_HANDLE_VALUE;
}

/**
 * Frees handles and resources allcated by createHandle().
 * @param  
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::freeHandle(void) //throw(...)
{
    /* release interface */
    int retval = usb_release_interface(stdHandle, USBFX2_INTFNO);
    if (retval != 0)
        return FAILED;

    /* close usb handle */
    retval = usb_close(stdHandle);
    if (retval != 0)
        return FAILED;

    /* all ok */
    return SUCCEED;
}

bool stdUSB::freeHandles(void) {
    // this function exits just because there is no reason
    // to open and close handle all time
    return SUCCEED;
}

/**
 * Frees handles and resources allcated by createHandle().
 * @param data -- command, control code, value, or such to be sent to the device
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::sendData(unsigned int data)// throw(...)
{
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
    int retval = usb_bulk_write(stdHandle, USBFX2_EP_WRITE, buff, sizeof(buff), USB_TOUT_MS);

//    printf("SendData: sent %d bytes\n", retval);

    if (retval == 4) //return value must be exact as the bytes transferred
        return SUCCEED;
    else
        return FAILED;
}

/**
 * Performs bulk transfer to IN end point. Reads data off the board.
 * @param pData -- pointer to input buffer data array
 * @param l -- size of the data buffer (think: pData[l] )
 * @param lread -- out param: number of samples read
 * @return bool -- SUCCEED or FAILED
 */
bool stdUSB::readData(unsigned short * pData, int l, int* lread)// throw(...)
{
    int buff_sz = l*sizeof(unsigned short);
    //char* buff = new char[110];
    //buff_sz = 110;

    /*  USBFX2_EP_READ => end point addr.
    buff => bytes to read (has to be <64 bytes)
    USB_TOUT_MS => timeout in ms
    */
    int retval = usb_bulk_read(stdHandle, USBFX2_EP_READ, (char*)pData, buff_sz, USB_TOUT_MS);

//    printf("readData: read %d bytes\n", retval);

    if (retval > 0) {
        //*lread = retval/buff_sz;
        *lread = (int)(retval / (unsigned long)sizeof(unsigned short));
        //*lread *= 4;
        return SUCCEED;
    } else
        printf("error code: %s\n", strerror(-1 * retval));
        *lread = retval;
        return FAILED;
}
