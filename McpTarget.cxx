#include "McpTarget.h"
#include "TargetData.h"

#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"

#include "stdUSB.h"

stdUSB fTheUsb;

McpTarget::McpTarget() 
  :fExtTrigMode(0),fEventNumber(0),fNumPedEvents(100)
{
  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int row=0;row<NUM_ROWS;row++) {
	for(int col=0;col<NUM_COLS;col++) {
	  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	    fPedestalValues[chip][chan][row][col][samp]=2000;
	  }
	}
      }
    }
  }
  loadDnlLookUpTable();
  loadPedestal();


  useSyncUsb(1);
  setTermValue(0,1,0);
  enablePedestal(false);
  setPedRowCol(0,0);
  setTrigPolarity(false); //Falling edge
  setWbias(1000); ///< ~1us allegedly
  setTrigThresh(1639); ///< 1.4v

}

McpTarget::~McpTarget()
{
}

Int_t McpTarget::justReadBuffer()
{
  //This only read the data from the USB thingy and sticks it into
  // fReadoutBuffer
  
  //Zero fReadoutBuffer
  memset(fReadoutBuffer,0,BUFFERSIZE*sizeof(unsigned short));

  int bytesRead=0;
  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.readData(fReadoutBuffer, BUFFERSIZE, &bytesRead);
  fTheUsb.freeHandles();

  if(fDumpRawHexData) {
    ofstream HexDump("eventHexDump.txt");
    for(int i=0;i<BUFFERSIZE;i++) {
      HexDump << std::dec << i << "\t" << std::hex << "\t" << fReadoutBuffer[i] << "\n";
    }
  }

  if(bytesRead<BUFFERSIZE) {	
    std::cerr << "Only read " << bytesRead << " of " << BUFFERSIZE << "\n";
    return -1;
  }


  return 0;
}


void McpTarget::generatePedestals() 
{
  Double_t tempValues[NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS][SAMPLES_PER_COL]={{{{{0}}}}};
  Int_t countStuff[NUM_ROWS][NUM_COLS]={{0}};
  
  Int_t row,col;
  TFile *fpTemp = new TFile("pedFile.root","RECREATE");
  TTree *pedTree = new TTree("pedTree","Tree of pedestal thingies");    
  TargetData *targetPtr=0;
  pedTree->Branch("target","TargetData",&targetPtr);
  pedTree->Branch("values",&fReadoutBuffer[0],"values[4140]/s");

  for(row=0;row<NUM_ROWS;row++) {
    for(col=0;col<NUM_COLS;col++) {	
      setPedRowCol(row,col);      
      
      for(int event=0;event<fNumPedEvents;event++) {
	//Send software trigger
	sendSoftTrig();
	usleep(1000);
	Int_t retVal=justReadBuffer();
	if(retVal<0) continue;
	//Now unpack the data into a more useful structure
	TargetData targetData(this->fReadoutBuffer);
	targetPtr=&targetData;
	pedTree->Fill();	
	countStuff[row][col]++;
	for(int chip=0;chip<NUM_TARGETS;chip++) {
	  for(int chan=0;chan<NUM_CHANNELS;chan++) {
	    for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	      Float_t value=(Float_t)targetData.DATA[chip][chan][samp];
	      tempValues[chip][chan][row][col][samp]=value;
	    }
	  }
	}
      }
    }
    std::cout << "Read " << fNumPedEvents << " events from " << row << "\n";
  }
        
  pedTree->AutoSave();
  delete fpTemp;
  
  std::ofstream PedFile("pedestal.txt");
  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int row=0;row<NUM_ROWS;row++) {
	for(int col=0;col<NUM_COLS;col++) {
	  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	    if(countStuff[row][col]>0) {
	      tempValues[chip][chan][row][col][samp]/=countStuff[row][col];
	      fPedestalValues[chip][chan][row][col][samp]=(Int_t)tempValues[chip][chan][row][col][samp];
	    }
	    PedFile << fPedestalValues[chip][chan][row][col][samp] << "\n";
	  }
	}
      }
    } 
  }
}

int McpTarget::readEvent()
{
  //Read data from USB class (stdUSB.cpp) since we dont have the device yet 
  //then we are going to read data from test data file 	
  static int counter=0;	
    

  useSyncUsb(0);
  setTermValue(0,1,0);
  enablePedestal(false);
  setPedRowCol(0,0);
  setTrigPolarity(false); //Falling edge
  setWbias(1000); ///< ~1us allegedly
  setTrigThresh(1639); ///< 1.4v
  

  
  sendSoftTrig();
  usleep(1000);   
    
  counter++;
  Int_t retVal=justReadBuffer();
  if(retVal<0) {
    return -1;
  }
  
  
  TargetData targetData(fReadoutBuffer);
  
  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	Double_t value = fDnlLUT[targetData.DATA[chip][chan][samp]];
	//DNL_LUT.txt
	value-=fPedestalValues[chip][chan][targetData.ROW_LOC[chip]][targetData.COL_LOC[chip]][samp];
	fVoltBuffer[chip][chan][samp]=value;
      }
    }
  }
  //  std::cout << "fPedSubbedBuffer[0][0] " << fPedSubbedBuffer[0][0] << "\n";

  fEventNumber++;
   
  return 1;
}

// void McpTarget::setExtTrigMode(int mode)
// {
//   if(fExtTrigMode==mode) return;
//   fExtTrigMode=mode;
//   if(fExtTrigMode) {

//  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
//    std::cerr << "USB failed to initalize.\n";
//    exit(0);
//  }
//     fTheUsb.sendData(0x10001);
// fTheUsb.freeHandles();
//   }
//   else {

//  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
//    std::cerr << "USB failed to initalize.\n";
//    exit(0);
//  }
//     fTheUsb.sendData(0x00001);

//  fTheUsb.freeHandles();
//     // do some dummy readouts before we continue
//     unsigned short int tmpData[5];
//     int read;
//     for(int i=0; i < 5; i++) {
//       usleep(10000);
//       fTheUsb.sendData(0x00002);
//       usleep(10000);
//       fTheUsb.readData(tmpData, 2, &read);
//     }
//   }

// }

TGraph *McpTarget::getChannel(int chip, int channel)
{
  if(chip<0 || chip>=NUM_TARGETS) return NULL;
  if(channel<0 || channel>=NUM_CHANNELS) return NULL;
  
  Double_t timeVals[SAMPLES_PER_COL]={0};
  Double_t voltVals[SAMPLES_PER_COL]={0};
  for(int i=0;i<SAMPLES_PER_COL;i++) {
     timeVals[i]=Double_t(i); //Should multiple by delta t here
     voltVals[i]=fVoltBuffer[chip][channel][i];
  }
  TGraph *gr = new TGraph(SAMPLES_PER_COL,timeVals,voltVals);
  return gr;
}


void McpTarget::loadPedestal() 
{
  Int_t value;
  std::ifstream PedFile("pedestal.txt");
  if(PedFile) {
    for(int chip=0;chip<NUM_TARGETS;chip++) {
      for(int chan=0;chan<NUM_CHANNELS;chan++) {
	for(int row=0;row<NUM_ROWS;row++) {
	  for(int col=0;col<NUM_COLS;col++) {
	    for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	      PedFile >> value;
	      fPedestalValues[chip][chan][row][col][samp]=value;
	    }
	  }
	}
      }
    }
    PedFile.close();
  }
}


void McpTarget::setPedRowCol(Int_t row, Int_t col)
{
  UInt_t dataVal=0;
  dataVal = PED_ROW_COL_MASK;
  dataVal |= (row << PED_ROW_SHIFT);
  dataVal |= (col << PED_COL_SHIFT);
  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.sendData(dataVal); 
  fTheUsb.freeHandles();
}

void McpTarget::enablePedestal(Int_t flag)
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  if(flag==1) {
    fTheUsb.sendData(ENABLE_PED_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_PED_MASK);
  }
  fTheUsb.freeHandles();
}


void McpTarget::setWbias(UInt_t value)
{
  UInt_t dataVal = WBIAS_MASK;
  dataVal |= (value << WBIAS_SHIFT);
  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.sendData(dataVal);
  fTheUsb.freeHandles();
}


void McpTarget::setTrigThresh(UInt_t value)
{
  UInt_t dataVal=TRIG_THRESH_MASK;
  dataVal |= (value&0xffff) << TRIG_THRESH_SHIFT;
  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.sendData(dataVal);
  fTheUsb.freeHandles();
}

void McpTarget::setTermValue(Int_t f100, Int_t f1k, Int_t f10k) 
{
  UInt_t dataVal=TERM_MASK;
  dataVal |= (f100&0x1) << TERM_100_OHMS_SHIFT;
  dataVal |= (f1k&0x1) << TERM_1K_OHMS_SHIFT;
  dataVal |= (f10k&0x1) << TERM_10K_OHMS_SHIFT;

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.sendData(dataVal);
  fTheUsb.freeHandles();

}


void McpTarget::setTrigPolarity(Int_t flag)
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  if(flag==1) {
    fTheUsb.sendData(TRIG_POLARITY_NEG);
  }
  else {
    fTheUsb.sendData(TRIG_POLARITY_POS);
  }
  fTheUsb.freeHandles();
}


void McpTarget:: useSyncUsb(Int_t flag)
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  if(flag==1) {
    fTheUsb.sendData(ENABLE_SYNC_USB_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_SYNC_USB_MASK);
  }
  fTheUsb.freeHandles();
}

void McpTarget:: sendSoftTrig()
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  fTheUsb.sendData(SOFT_TRIG_MASK);
}


void McpTarget::loadDnlLookUpTable()
{
  std::ifstream DNL("DNL_LUT.txt");
  int sampVal;
  double lutVal;
  for(int i=0;i<4096;i++) {
    DNL >> sampVal >> lutVal;
    fDnlLUT[sampVal]=lutVal;
  }
}

// void McpTarget::getMemAddress(UInt_t memAddrSpace, UInt_t &rowLoc, UInt_t &colLoc,
// 			   UInt_t &pixLoc)
// {
//   const unsigned int MASK_COL = 0x000001F0;
//   const unsigned int MASK_ROW = 0x00000E00;

//   colLoc = memAddrSpace & MASK_COL;
//   rowLoc = memAddrSpace & MASK_ROW;
    
//   pixLoc = colLoc;
//   colLoc = colLoc >> 4;
//   rowLoc = rowLoc >> 9;

// }


void McpTarget::rawSendInt(unsigned int value)
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  bool retVal=fTheUsb.sendData(value);
  fTheUsb.freeHandles();
  std::cout << "Got " << retVal << " for sending " << value << "\n";
}


void McpTarget::rawReadInts(int numInts, unsigned short buffer[])
{

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
    exit(0);
  }
  int numRead=0;
  int retVal=fTheUsb.readData(buffer,numInts,&numRead);
  std::cout << "Got " << retVal << " for reading " << numRead 
	    << " of " << numInts << " values " << "\n";
  if(numRead>0) {
    for(int i=0;i<numRead;i++) {
      std::cout << i << "\t" << buffer[i] << "\n";
    }
  }
  fTheUsb.freeHandles();
}
