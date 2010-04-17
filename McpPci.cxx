#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>     
#include <sys/ioctl.h>
#include "testDriver_ioctl.h"
#include "McpPci.h"

#define MCP_DEV "/dev/MCP_cPCI"

McpPci::McpPci(void)
{
  //  createHandles();
}
McpPci::~McpPci(void)
{
}


bool McpPci::createHandles(void) 
{
  if(!fTheFd)
    fTheFd = open(MCP_DEV, O_RDWR | O_NONBLOCK);
  if(fTheFd>0)
    return SUCCEED;
  std::cerr << "Couldn't open " << MCP_DEV << "\t" << strerror(errno) << "\n";
  return FAILED;
}
bool McpPci::freeHandles(void)
{
  return SUCCEED;
}

bool McpPci::freeHandle(void)
{
  if(fTheFd) {
    int retVal=close(fTheFd);
    if(retVal<0) {
      std::cerr << "Couldn't close " << MCP_DEV << "\t" << strerror(errno) << "\n";
    }
  }
  return SUCCEED;
}

bool McpPci::sendData(unsigned int data)
{
  if(!fTheFd) createHandles();
 int retval= 0;
 struct testDriverRead req;
 // std::cout << std::hex << data << "\n";
 req.address = 0x1c;
 req.barno = 1;
 req.value = data;
 if (ioctl(fTheFd, TEST_IOCWRITE, &req)) {
   perror("ioctl");
   close(fTheFd);
   exit(1);
 }
   //   retval = write(fTheFd, &data, sizeof(unsigned int));   
//    if (retval < 0) {    
//      printf("Error sending command!\n");      
//      if (errno != EAGAIN) {
//        perror("Error writing:");
//        return FAILED;
//      } 
//    }
 //}
 return SUCCEED;
}

bool McpPci::readData(unsigned short * pData, int length, int* lread)
{
  if(!fTheFd) createHandles();
  static  unsigned int bigBuffer[100000];
  int newLength = length + 40;
  int startWord = 1;
  int retval;  
  retval = 0;
  while (retval <= 0) {    
    retval = read(fTheFd, bigBuffer, sizeof(unsigned int)*newLength);
    if (retval < 0) {      
      if (errno != EAGAIN) {	
	perror("Error reading:");	
	return FAILED;
      } 
    }
    *lread=length;
    for(int i=0;i<length;i++) {
      pData[i]=bigBuffer[i+startWord]&0xffffffff;
    }
  }
  return SUCCEED;
}
