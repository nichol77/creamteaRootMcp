#include "McpTarget.h"
#include "TargetData.h"

#include <iostream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"


#include "McpPci.h"
#ifndef READOUT_MCP_CPCI
#include "stdUSB.h"
#endif


#ifdef READOUT_MCP_CPCI
McpPci fThePci;
#else
stdUSB fTheUsb;
#endif

McpTarget::McpTarget(int offlineMode) 
  :fOfflineMode(offlineMode),fExtTrigMode(0),fEventNumber(0),fNumPedEvents(100)
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
  fSoftTrigMode=0;
  loadDnlLookUpTable();
  loadPedestal();
  fTargetDataPtr=0;
  fRawTargetDataPtr=0;
  fTheOutputFile=0;
  fTheOutputTree=0;
  fOutputMode=0;

  if(!fOfflineMode) {
    std::cerr << "Trying to setup something\n";
#ifdef READOUT_MCP_CPCI
#else
    
    if (fTheUsb.createHandles() != true) {
      std::cerr << "USB failed to initalize.\n";
      exit(0);
    }
#endif
    
    useSyncUsb(0);
    setTermValue(0,1,0);
    enablePedestal(false);
    setPedRowCol(0,0);
    setTrigPolarity(false); //Falling edge
    setWbias(800); ///< ~1us allegedly
    setTrigThresh(1639); ///< 1.4v
  }
}

McpTarget::~McpTarget()
{
  saveOutputFile();
}

void McpTarget::saveOutputFile()
{
  if(fOutputMode) {
    if(fTheOutputTree)
      fTheOutputTree->AutoSave();
  }
}

Int_t McpTarget::justReadBuffer()
{
  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't read buffer\n";
    return 0;
  }
  //This only read the data from the USB thingy and sticks it into
  // fReadoutBuffer
  
  //Zero fReadoutBuffer
  memset(fReadoutBuffer,0,BUFFERSIZE*sizeof(unsigned short));

  int bytesRead=0;
#ifdef READOUT_MCP_CPCI
  bool retVal=fThePci.readData(fReadoutBuffer, BUFFERSIZE, &bytesRead);
  fThePci.sendData(CLEAR_EVENT_CPCI_BOARD_MASK);
  if(retVal!=true) { 
    sleep(1);
    retVal=fThePci.readData(fReadoutBuffer, BUFFERSIZE, &bytesRead);
    fThePci.sendData(CLEAR_EVENT_CPCI_BOARD_MASK);
  }

#else
  bool retVal=fTheUsb.readData(fReadoutBuffer, BUFFERSIZE, &bytesRead);
  if(retVal!=true) { 
    sleep(1);
    retVal=fTheUsb.readData(fReadoutBuffer, BUFFERSIZE, &bytesRead);
  }
#endif

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
  //  std::cerr << "Here\n";
  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't generate pedestals\n";
    return;
  }
  Double_t *tempValues[NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS];
  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int row=0;row<NUM_ROWS;row++) {
	for(int col=0;col<NUM_COLS;col++) {
	  tempValues[chip][chan][row][col]= new Double_t [SAMPLES_PER_COL];
	  for(int samp=0;samp>SAMPLES_PER_COL;samp++)
	    tempValues[chip][chan][row][col][samp]=0;
	}
      }
    }
  }
  Int_t countStuff[NUM_ROWS][NUM_COLS]={{0}};
  
  Int_t row,col;
  TFile *fpTemp = new TFile("pedFile.root","RECREATE");
  TTree *pedTree = new TTree("pedTree","Tree of pedestal thingies");    
  pedTree->Branch("target","RawTargetData",&fRawTargetDataPtr);
  pedTree->Branch("values",&fReadoutBuffer[0],"values[32810]/s");
  enablePedestal(1);
  
  for(row=0;row<NUM_ROWS;row++) {
    for(col=0;col<NUM_COLS;col++) {	
//  for(row=0;row<1;row++) {
//    for(col=0;col<1;col++) {	
      setPedRowCol(row,col);      
      
      for(int event=0;event<fNumPedEvents;event++) {
	//Send software trigger
	if(event%100==0) std::cerr << "*";
	sendSoftTrig();
#ifndef READOUT_MCP_CPCI
	usleep(1000);
#endif
	Int_t retVal=justReadBuffer();
	if(retVal<0) continue;
	//Now unpack the data into a more useful structure
	TargetData targetData(this->fReadoutBuffer);
	RawTargetData rawTargetData(this->fReadoutBuffer);
	fTargetDataPtr=&targetData;
	fRawTargetDataPtr=&rawTargetData;
	pedTree->Fill();	
	countStuff[row][col]++;

	for(int chip=0;chip<NUM_TARGETS;chip++) {

	  Int_t readRow=fTargetDataPtr->rowLoc[chip];
	  Int_t readCol=fTargetDataPtr->colLoc[chip];
	  if(row!=readRow || col!=readCol) {
	    std::cerr << "Oops: " << chip << "\t" << row  << "\t" << col << "\t" 
		      << readRow << "\t" << readCol << "\n";
	  }

	  for(int chan=0;chan<NUM_CHANNELS;chan++) {
	    for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	      
	      Double_t valueInt=targetData.data[chip][chan][samp];
	      
	      if(chip==0 && chan==0 && samp==0) {
		if(targetData.raw[1]==32768) {
		  std::cout << chip << "\t" << chan << "\t" << row << "\t" << col << "\t"
			    << samp << "\t" << valueInt << "\t" << fPedestalValues[chip][chan][row][col][samp]
			    << "\n";
		}
	      }

	      Float_t value=(Float_t)targetData.data[chip][chan][samp];
	      tempValues[chip][chan][row][col][samp]+=value;
	    }
	  }
	}
      }
      std::cerr << "\n";
    }
    std::cout << "Read " << fNumPedEvents << " events from " << row << "\n";
  }
  
  std::ofstream PedFile("pedestal.txt");
  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int row=0;row<NUM_ROWS;row++) {
	for(int col=0;col<NUM_COLS;col++) {
	  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
	    if(countStuff[row][col]>0) {
	      tempValues[chip][chan][row][col][samp]/=countStuff[row][col];
	      fPedestalValues[chip][chan][row][col][samp]=(Int_t)tempValues[chip][chan][row][col][samp];
	      if(chip==0 && chan==0 && row==0 && col==0 && samp==0)
		std::cout << fPedestalValues[chip][chan][row][col][samp] << "\n";
	    }
	    PedFile << fPedestalValues[chip][chan][row][col][samp] << "\n";
	  }
	  delete [] tempValues[chip][chan][row][col];
	}
      }
    } 
  }

        
  pedTree->AutoSave();
  //  delete fpTemp;
}

int McpTarget::readEvent()
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't read event\n";
    return 0;
  }
  //Read data from USB class (stdUSB.cpp) since we dont have the device yet 
  //then we are going to read data from test data file 	
  static int counter=0;	
    

  if(counter<2) {
    useSyncUsb(0);
    setTermValue(0,1,0);
    enablePedestal(false);
    setPedRowCol(0,0);
    setTrigPolarity(false); //Falling edge
    setWbias(800); ///< ~1us allegedly
    setTrigThresh(1639); ///< 1.4v
  }

  if(fSoftTrigMode) {
    sendSoftTrig();
  }
    
  counter++;
  Int_t retVal=justReadBuffer();
  if(retVal<0) {
    return -1;
  }
  
  if(fTargetDataPtr)
    delete fTargetDataPtr;  
  fTargetDataPtr = new TargetData(fReadoutBuffer);
  if(fOutputMode) {
    if(fRawTargetDataPtr) 
      delete fRawTargetDataPtr;
    fRawTargetDataPtr = new RawTargetData(fReadoutBuffer);
    fTheOutputTree->Fill();
  }

  //Do the pedestal subtraction and voltage conversion
  fillVoltageArray(fTargetDataPtr);

  //  std::cout << "fPedSubbedBuffer[0][0] " << fPedSubbedBuffer[0][0] << "\n";

  fEventNumber++;
   
  return 1;
}

void McpTarget::fillVoltageArray(TargetData *targetDataPtr)
{

  for(int chip=0;chip<NUM_TARGETS;chip++) {
    for(int chan=0;chan<NUM_CHANNELS;chan++) {
      for(int samp=0;samp<SAMPLES_PER_COL;samp++)  {
// 	std::cerr << chip << "\t" << chan << "\t" << samp << "\t"
// 		  << targetDataPtr->data[chip][chan][samp] << "\n";
	  
//	Double_t value = fDnlLUT[targetDataPtr->data[chip][chan][samp]];
	Double_t value=targetDataPtr->data[chip][chan][samp];
	//	if(value<800) {
	//	  std::cout << chip  << "\t" << chan << "\t" << samp << "\t"
	//		    << value << "\n";
	//	}
// 	std::cerr << chip << "\t" << chan << "\t" << samp << "\t"
// 		  << targetDataPtr->data[chip][chan][samp] << "\t"
// 		  << value << "\t" << targetDataPtr->rowLoc[chip]
// 		  <<"\t" << targetDataPtr->colLoc[chip] << "\n";
	Int_t row=targetDataPtr->rowLoc[chip];
	Int_t col=targetDataPtr->colLoc[chip];
// 	std::cerr << row << "\t" << col << "\t" << NUM_ROWS << "\t"
// 		  << NUM_COLS << "\n";
// 	std::cerr << fPedestalValues[chip][chan][row][col][samp] << "\n";
	if(chip==3 && chan==11 && samp==160) {
	  //  if(targetDataPtr->raw[1]==32768) {
	  if(row==2 && col==28) {
	    std::cout << chip << "\t" << chan << "\t" << row << "\t" << col << "\t"
		      << samp << "\t" << value << "\t" << fPedestalValues[chip][chan][row][col][samp]
		      << "\n";
	  }
	}

	value-=fPedestalValues[chip][chan][row][col][samp]; 
	

	fVoltBuffer[chip][chan][samp]=value;
	targetDataPtr->fVoltBuffer[chip][chan][samp]=value;
	//	exit(0);
	//DNL_LUT.txt 
      }
    }
  }
}

// void McpTarget::setExtTrigMode(int mode)
// {
//   if(fExtTrigMode==mode) return;
//   fExtTrigMode=mode;
//   if(fExtTrigMode) {


//     fTheUsb.sendData(0x10001);
//   }
//   else {


//     fTheUsb.sendData(0x00001);

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
  std::ifstream PedFile;
  PedFile.open("pedestal.txt");
  if(!PedFile.is_open()) {
    PedFile.open("defaultPed.txt");
  }
    
  if(PedFile.is_open()) {
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

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't set ped row col\n";
    return;
  }
  UInt_t dataVal=0;
  dataVal = PED_ROW_COL_MASK;
  dataVal |= (row << PED_ROW_SHIFT);
  dataVal |= (col << PED_COL_SHIFT);

#ifdef READOUT_MCP_CPCI
  fThePci.sendData(dataVal);
#else
  fTheUsb.sendData(dataVal); 
#endif
}

void McpTarget::enablePedestal(Int_t flag)
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't enable pedestal\n";
    return;
  }



#ifdef READOUT_MCP_CPCI
  if(flag==1) {
    fThePci.sendData(ENABLE_PED_MASK);
  }
  else {
    fThePci.sendData(DISABLE_PED_MASK);
  }
#else
  if(flag==1) {
    fTheUsb.sendData(ENABLE_PED_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_PED_MASK);
  }
#endif
}


void McpTarget::setWbias(UInt_t value)
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't set WBIAS\n";
    return;
  }
  UInt_t dataVal = WBIAS_MASK;
  dataVal |= (value << WBIAS_SHIFT);

#ifdef READOUT_MCP_CPCI
  fThePci.sendData(dataVal);
#else
  fTheUsb.sendData(dataVal);
#endif
}


void McpTarget::setTrigThresh(UInt_t value)
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't set trigger threshold\n";
    return;
  }
  UInt_t asicMasks[4]={0x0000,0x4000,0x8000,0xc000};

  for(int asic=0;asic<NUM_TARGETS;asic++) {
    UInt_t dataVal=TRIG_THRESH_MASK;
    UInt_t tempValue=value|asicMasks[asic];
    dataVal |= (tempValue&0xffff) << TRIG_THRESH_SHIFT;

#ifdef READOUT_MCP_CPCI
    fThePci.sendData(dataVal);
#else
    fTheUsb.sendData(dataVal);
#endif
  }
  fThresholdValue=value;

}

void McpTarget::setTermValue(Int_t f100, Int_t f1k, Int_t f10k) 
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't set TERM value\n";
    return;
  }
   UInt_t dataVal=TERM_BASE;
   dataVal |= ((f100&0x1) << TERM_100_OHMS_SHIFT);
   dataVal |= ((f1k&0x1) << TERM_1K_OHMS_SHIFT);
   dataVal |= ((f10k&0x1) << TERM_10K_OHMS_SHIFT);
//   unsigned int dataVal=0;
//   //dataVal = f10k << 16 & TERM_MASK;      //10kohms
//   //dataVal = f1k  << 17 & dataVal;//1kohms
//   //dataVal = f100 << 18 & dataVal;//100ohms
//   std::cout << f100 << "\t" << f1k << "\t" << f10k << "\n";
//   std::cout << std::hex << dataVal << "\n";



#ifdef READOUT_MCP_CPCI
  fThePci.sendData(dataVal);

#else
  fTheUsb.sendData(dataVal);
  //  fTheUsb.freeHandles();
#endif

}


void McpTarget::setTrigPolarity(Int_t flag)
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't send trigger polarity\n";
    return;
  }

#ifdef READOUT_MCP_CPCI
  if(flag==1) {
    fThePci.sendData(TRIG_POLARITY_NEG);
  }
  else {
    fThePci.sendData(TRIG_POLARITY_POS);
  }

#else
  if(flag==1) {
    fTheUsb.sendData(TRIG_POLARITY_NEG);
  }
  else {
    fTheUsb.sendData(TRIG_POLARITY_POS);
  }
#endif
}


void McpTarget:: useSyncUsb(Int_t flag)
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't send USB sync\n";
    return;
  }


#ifdef READOUT_MCP_CPCI
  if(flag==1) {
    fThePci.sendData(ENABLE_SYNC_USB_MASK);
  }
  else {
    fThePci.sendData(DISABLE_SYNC_USB_MASK);
  }

#else
  if(flag==1) {
    fTheUsb.sendData(ENABLE_SYNC_USB_MASK);
  }
  else {
    fTheUsb.sendData(DISABLE_SYNC_USB_MASK);
  }
#endif
}

void McpTarget:: sendSoftTrig()
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't send software trigger\n";
    return;
  }

#ifdef READOUT_MCP_CPCI
  fThePci.sendData(SOFT_TRIG_MASK);
#else
  fTheUsb.sendData(SOFT_TRIG_MASK);
#endif
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

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't sent int\n";
    return;
  }

#ifdef READOUT_MCP_CPCI

  bool retVal=fThePci.sendData(value);
#else
  bool retVal=fTheUsb.sendData(value);
#endif
  std::cout << "Got " << retVal << " for sending " << value << "\n";
}


void McpTarget::rawReadInts(int numInts, unsigned short buffer[])
{

  if(fOfflineMode) {
    std::cerr << "Running in offline mode can't read buffer\n";
    return;
  }
  int numRead=0;
#ifdef READOUT_MCP_CPCI
  int retVal=fThePci.readData(buffer,numInts,&numRead);
  fThePci.sendData(CLEAR_EVENT_CPCI_BOARD_MASK);
#else
  int retVal=fTheUsb.readData(buffer,numInts,&numRead);
#endif
  std::cout << "Got " << retVal << " for reading " << numRead 
	    << " of " << numInts << " values " << "\n";
  if(numRead>0) {
    for(int i=0;i<numRead;i++) {
      std::cout << i << "\t" << buffer[i] << "\n";
    }
  }
}


void McpTarget::openOutputFile(char fileName[180])
{
  fOutputMode=1;
  fTheOutputFile = new TFile(fileName,"RECREATE");
  fTheOutputTree = new TTree("mcpTree","Target Output Tree");
  fTheOutputTree->Branch("target","RawTargetData",&fRawTargetDataPtr);
}
