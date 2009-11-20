#include "Target.h"
#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"

#include "stdUSB.h"

stdUSB fTheUsb;

Target::Target() 
  :fExtTrigMode(0),fEventNumber(0),fNumPedEvents(100)
{
  for(int chan=0;chan<NUM_CHANNELS;chan++) {
    for(int row=0;row<NUM_ROWS;row++) {
      for(int col=0;col<NUM_COLS;col++) {
	for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	  fPedestalValues[chan][row][col][samp]=2000;
	}
      }
    }
  }
  loadPedestal();

  if (fTheUsb.createHandles() != stdUSB::SUCCEED) {
    std::cerr << "USB failed to initalize.\n";
  }

  useSyncUsb(0);
  setTermValue(1,0,0);
  enablePedestal(false);
  setPedRowCol(0,0);
  asumOrDtrig(true);
  setTrigPolarity(1);
  setWbias(1000); ///< ~1us allegedly
  setAsumThresh(2294); ///< 1.4v
  setTrigThresh(1); ///< 1.4v
  useEventCounter(1);
  setPedRowCol(0,0);

}

Target::~Target()
{
  fTheUsb.freeHandles();
}

void Target::generatePedestals() 
{
  short unsigned int *tmpBuffer = &fReadoutBuffer[0];
  int bytesRead;
  Double_t tempValues[NUM_CHANNELS][NUM_ROWS][NUM_COLS][SAMPLES_PER_COL]={{{{0}}}};
  Int_t countStuff[NUM_ROWS][NUM_COLS]={{0}};
  


  UInt_t rowLoc,colLoc,pixLoc;
  Int_t row,col;
  TFile *fpTemp = new TFile("pedFile.root","RECREATE");
  TTree *pedTree = new TTree("pedTree","Tree of pedestal thingies");    
  pedTree->Branch("values",&fReadoutBuffer[0],"values[1040]/s");
  pedTree->Branch("row",&rowLoc,"row/I");
  pedTree->Branch("col",&colLoc,"col/I");

  for(row=0;row<NUM_ROWS;row++) {
    for(col=0;col<NUM_COLS;col++) {	
      //      setPedRowCol(row,col);      
      for(int event=0;event<fNumPedEvents;event++) {
	//Send software trigger
	sendSoftTrig();

	fTheUsb.readData(tmpBuffer, BUFFERSIZE, &bytesRead);
	if(bytesRead<BUFFERSIZE) {	
	  std::cerr << "Only read " << bytesRead << " of " << BUFFERSIZE << "\n";
	  continue;
	}
	pedTree->Fill();
     
		
	getMemAddress(tmpBuffer[2],rowLoc,colLoc,pixLoc);
	//	std::cout << "Row Col Pix\t" << rowLoc << "\t" << colLoc << "\t" << pixLoc << "\n";
	//	std::cout << "tmpBuffer[3] " << tmpBuffer[3] << "\t" << fPedestalValues[0][rowLoc][colLoc][0] << "\n";
	countStuff[rowLoc][colLoc]++;
	for(int chan=0;chan<NUM_CHANNELS;chan++) {
	  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	    int i=3+ SAMPLES_PER_COL*chan + samp;
	    tempValues[chan][rowLoc][colLoc][samp]+=tmpBuffer[i];
	  }
	}
      }
    
    }  
    std::cout << "Read " << fNumPedEvents << " events from " << row << "\n";
    
    
  }
  pedTree->AutoSave();
  delete fpTemp;
  
  std::ofstream PedFile("pedestal.txt");
  for(int chan=0;chan<NUM_CHANNELS;chan++) {
    for(int row=0;row<NUM_ROWS;row++) {
      for(int col=0;col<NUM_COLS;col++) {
	for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	  if(countStuff[row][col]>0) 
	    tempValues[chan][row][col][samp]/=countStuff[row][col];
	  fPedestalValues[chan][row][col][samp]=(Int_t)tempValues[chan][row][col][samp];
	  PedFile << fPedestalValues[chan][row][col][samp] << "\n";
	}
      }
    }
  } 
}

int Target::readEvent()
{
  //Read data from USB class (stdUSB.cpp) since we dont have the device yet 
  //then we are going to read data from test data file 	
  short unsigned int *tmpBuffer = &fReadoutBuffer[0];
  int bytesRead;
  UInt_t rowLoc,colLoc,pixLoc;
  static int counter=0;	
  //  setPedRowCol(0,0);      
  //  usleep(1000);
  //  setPedRowCol(1,counter%32);
  
   
  sendSoftTrig();
    //    usleep(1000);
    
  counter++;
  fTheUsb.readData(tmpBuffer, BUFFERSIZE, &bytesRead);
  if(bytesRead<BUFFERSIZE) {
    //Couldn't read an event
    
    std::cerr << "Only read " << bytesRead << " of " << BUFFERSIZE << "\n";
    return -1;
  }
  
    //  for(int i=0;i<BUFFERSIZE;i++) {
    //    std::cout << i << "\t" << tmpBuffer[i] << "\n";
    //  }
  
  getMemAddress(tmpBuffer[2],rowLoc,colLoc,pixLoc);
  //  std::cout << "Row Col Pix\t" << rowLoc << "\t" << colLoc << "\t" << pixLoc << "\n";
  //  std::cout << "tmpBuffer[3] " << tmpBuffer[3] << "\t" << fPedestalValues[0][rowLoc][colLoc][0] << "\n";
  for(int chan=0;chan<NUM_CHANNELS;chan++) {
    for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
      int i=3+samp + chan*SAMPLES_PER_COL;	
      fPedSubbedBuffer[chan][samp]=int(tmpBuffer[i])-fPedestalValues[chan][rowLoc][colLoc][samp];      
      fVoltBuffer[chan][samp]=fPedSubbedBuffer[chan][samp]*VOLTS_PER_COUNT;
    }
  }
  //  std::cout << "fPedSubbedBuffer[0][0] " << fPedSubbedBuffer[0][0] << "\n";

  fEventNumber++;
  fTargetEventNumber=tmpBuffer[1];
  
  return 1;
}

void Target::setExtTrigMode(int mode)
{
  if(fExtTrigMode==mode) return;
  fExtTrigMode=mode;
  if(fExtTrigMode) {
    fTheUsb.sendData(0x10001);
  }
  else {
    fTheUsb.sendData(0x00001);
    // do some dummy readouts before we continue
    unsigned short int tmpData[5];
    int read;
    for(int i=0; i < 5; i++) {
      usleep(10000);
      fTheUsb.sendData(0x00002);
      usleep(10000);
      fTheUsb.readData(tmpData, 2, &read);
    }
  }

}

TGraph *Target::getChannel(int channel)
{
  if(channel<0 || channel>=NUM_CHANNELS) return NULL;
  
  Double_t timeVals[SAMPLES_PER_COL]={0};
  Double_t voltVals[SAMPLES_PER_COL]={0};
  for(int i=0;i<SAMPLES_PER_COL;i++) {
     timeVals[i]=Double_t(i); //Should multiple by delta t here
    voltVals[i]=fVoltBuffer[channel][i];
  }
  TGraph *gr = new TGraph(SAMPLES_PER_COL,timeVals,voltVals);
  return gr;
}


void Target::loadPedestal() 
{
  Int_t value;
  std::ifstream PedFile("pedestal.txt");
  if(PedFile) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int row=0;row<NUM_ROWS;row++) {
	for(int col=0;col<NUM_COLS;col++) {
	  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	    PedFile >> value;
	    fPedestalValues[chan][row][col][samp]=value;
	  }
	}
      }
    }
  }
}



void Target::setPedRowCol(Int_t row, Int_t col)
{
  UInt_t dataVal=0;
  dataVal = PED_ROW_COL_MASK;
  dataVal |= (row << PED_ROW_SHIFT);
  dataVal |= (col << PED_COL_SHIFT);
  fTheUsb.sendData(dataVal); 
}

void Target::enablePedestal(Int_t flag)
{
  if(flag==1) {
    fTheUsb.sendData(ENABLE_PED_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_PED_MASK);
  }
}

void Target::asumOrDtrig(Int_t flag)
{
  if(flag==1) {
    fTheUsb.sendData(ENABLE_ASUM_MASK);
  }
  else {
    fTheUsb.sendData(ENABLE_DTRIG_MASK);
  } 
  
}

void Target::setWbias(UInt_t value)
{
  UInt_t dataVal = WBIAS_MASK;
  dataVal |= (value << WBIAS_SHIFT);
  fTheUsb.sendData(dataVal);
}

void Target::setAsumThresh(UInt_t value) 
{
  UInt_t dataVal=ASUM_THRESH_MASK;
  dataVal |= (value&0xffff) << TRIG_THRESH_SHIFT;
  fTheUsb.sendData(dataVal);
}


void Target::setTrigThresh(UInt_t value)
{
  UInt_t dataVal=TRIG_THRESH_MASK;
  dataVal |= (value&0xffff) << TRIG_THRESH_SHIFT;
  fTheUsb.sendData(dataVal);
}

void Target::setTermValue(Int_t f100, Int_t f1k, Int_t f10k) 
{
  UInt_t dataVal=TERM_MASK;
  dataVal |= (f100&0x1) << TERM_100_OHMS_SHIFT;
  dataVal |= (f1k&0x1) << TERM_1K_OHMS_SHIFT;
  dataVal |= (f10k&0x1) << TERM_10K_OHMS_SHIFT;
  fTheUsb.sendData(dataVal);

}


void Target::setTrigPolarity(Int_t flag)
{
  if(flag==1) {
    fTheUsb.sendData(TRIG_POLARITY_NEG);
  }
  else {
    fTheUsb.sendData(TRIG_POLARITY_POS);
  }
}


void Target:: useSyncUsb(Int_t flag)
{
  if(flag==1) {
    fTheUsb.sendData(ENABLE_SYNC_USB_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_SYNC_USB_MASK);
  }
}

void Target:: sendSoftTrig()
{
  fTheUsb.sendData(SOFT_TRIG_MASK);
}

void Target:: useEventCounter(Int_t flag)
{
  if(flag==1) {
    fTheUsb.sendData(ENABLE_EVENT_COUNTER_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_EVENT_COUNTER_MASK);
  }
}


void Target::getMemAddress(UInt_t memAddrSpace, UInt_t &rowLoc, UInt_t &colLoc,
			   UInt_t &pixLoc)
{
  const unsigned int MASK_COL = 0x000001F0;
  const unsigned int MASK_ROW = 0x00000E00;

  colLoc = memAddrSpace & MASK_COL;
  rowLoc = memAddrSpace & MASK_ROW;
    
  pixLoc = colLoc;
  colLoc = colLoc >> 4;
  rowLoc = rowLoc >> 9;

}
