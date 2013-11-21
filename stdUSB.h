/*****
 
stdUSBL - A libusb implementation of ezusbsys.

StdUSB libusb implementation used here uses same function interface with native stdUSB Windows WDM driver.
 
*****/

//#include <stdio.h>
#include <usb.h>
#include <stdint.h>

#define MAX_STDUSB_DEVICES 4

class stdUSB {
public:
    stdUSB(void);
    ~stdUSB(void);
    int getNumHandles() {return numHandles;}
    int countDevices(void);
    bool createHandles(int *busNums, int *devNums, int expNum);
    bool createHandle(int usbDevInd, int busNum=-1,int devNum=-1);
    bool freeHandles(void);;
    bool freeHandle(int usbDevInd);
    //bool sendData(unsigned short data);
    bool sendData(unsigned int data,int whichDev=0);
    bool readData(unsigned short * pData, int l, int* lread); //Reads from all
    bool readData(unsigned short * pData, int l, int* lread,int usbDevInd); //Reads from one

    static const bool SUCCEED = true;
    static const bool FAILED  = false;
private:
    struct usb_device* init(int usbDevInd, int busNum=-1,int devNum=-1);

    #define INVALID_HANDLE_VALUE NULL
    #define USB_TOUT_MS 2000 //200 ms
    //#define USB_TOUT_MS 50  // 50 ms
    #define USBFX2_EP_WRITE 0x02 //USBFX2 end point address for bulk write
    #define USBFX2_EP_READ 0x86 //USBFX2 end point address for bulk read
    #define USBFX2_INTFNO 0 //USBFX2 interface number
    #define USBFX2_CNFNO 1 //USBFX2 configuration number

    /* USBFX2 device descriptions */
    static const uint16_t USBFX2_VENDOR_ID = 0x6672; //0x090c;
    static const uint16_t USBFX2_PRODUCT_ID = 0x2920; //0x1000;
protected:
    /* The handle to the usb device. Needed by write & read operations. */
    struct usb_dev_handle* stdHandle[MAX_STDUSB_DEVICES];
    int numHandles;
};
