////////////////////////////////////////////////////////////////////////////////
//////// Quickly thrown together ROOT code to read out MCPTARGET             //////
//////// rjn@hep.ucl.ac.uk                                                //////
////////////////////////////////////////////////////////////////////////////////

#ifndef MCPTARGET
#define MCPTARGET
//ROOT includes
#include "TGraph.h"

//rootTarget includes
#include "Defs.h"
#include "TargetData.h"




class McpTarget 
{
 public:
  McpTarget();
  ~McpTarget();

  void setDumpHexData(int value) {fDumpRawHexData=value;}
  void rawSendInt(unsigned int value);
  void rawReadInts(int numInts, unsigned short buffer[]);
  void generatePedestals();
  void loadPedestal();
  void loadDnlLookUpTable();
  Int_t readEvent();
  Int_t justReadBuffer();
  void setExtTrigMode(Int_t mode);
  void useSyncUsb(Int_t flag);
  void sendSoftTrig();
  void setPedRowCol(Int_t row, Int_t col);
  void enablePedestal(Int_t flag);
  void setWbias(UInt_t value);
  void setTrigThresh(UInt_t value);
  void setTermValue(Int_t f100, Int_t f1k, Int_t f10k);
  void setTrigPolarity(Int_t flag); ///< 1 is negative, 0 is positive
  //  void getMemAddress(UInt_t memAddrSpace, UInt_t &rowLoc, UInt_t &colLoc,
  //		     UInt_t &pixLoc);

  TGraph *getChannel(Int_t chanInd) 
  { 
    return getChannel(chanInd/NUM_CHANNELS,
		      chanInd%NUM_CHANNELS);
  }
  TGraph *getChannel(Int_t chip, Int_t channel);

  TargetData *getTargetData() {return fTargetDataPtr;}

 private:
  Int_t fDumpRawHexData; ///< Whether or not to dump out the raw hex data 
  Int_t fTermMode; ///< 100, 1k, 10k 
  Int_t fSampMode; ///< Unclearxs
  Int_t fExtTrigMode; ///< On or off
  Int_t fEventNumber; ///< Software event number
  Int_t fTargetEventNumber; ///< Event counter from Target??
  Int_t fNumPedEvents; ///< Number of pedestal events
  UShort_t fBuffer[BUFFERSIZE]; ///< Arbitrarily sized buffer
  UShort_t fReadoutBuffer[BUFFERSIZE+2]; ///<Arbitrarily sized bigger buffer
  Int_t fPedSubbedBuffer[NUM_TARGETS][NUM_CHANNELS][SAMPLES_PER_COL]; ///<Again who knows or dares to dream
  Float_t fDnlLUT[4096]; ///< No idea what this is, or why it is 4096
  Float_t fPedestalValues[NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS][SAMPLES_PER_COL];
  Float_t fVoltBuffer[NUM_TARGETS][NUM_CHANNELS][SAMPLES_PER_COL];
  Float_t fDNLLookUpTable[4096];
  TargetData *fTargetDataPtr;

};


#endif //MCPTARGET
