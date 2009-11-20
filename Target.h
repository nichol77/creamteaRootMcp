////////////////////////////////////////////////////////////////////////////////
//////// Quickly thrown together ROOT code to read out TARGET             //////
//////// rjn@hep.ucl.ac.uk                                                //////
////////////////////////////////////////////////////////////////////////////////

#ifndef TARGET
#define TARGET
//ROOT includes
#include "TGraph.h"

//rootTarget includes
#include "Defs.h"




class Target 
{
 public:
  Target();
  ~Target();

  void generatePedestals();
  void loadPedestal();
  Int_t readEvent();
  void setExtTrigMode(Int_t mode);
  void useSyncUsb(Int_t flag);
  void sendSoftTrig();
  void useEventCounter(Int_t flag);
  void setPedRowCol(Int_t row, Int_t col);
  void enablePedestal(Int_t flag);
  void asumOrDtrig(Int_t flag);
  void setWbias(UInt_t value);
  void setAsumThresh(UInt_t value);
  void setTrigThresh(UInt_t value);
  void setTermValue(Int_t f100, Int_t f1k, Int_t f10k);
  void setTrigPolarity(Int_t flag); ///< 1 is negative, 0 is positive
  void getMemAddress(UInt_t memAddrSpace, UInt_t &rowLoc, UInt_t &colLoc,
		     UInt_t &pixLoc);

  TGraph *getChannel(Int_t channel);


 private:
  Int_t fTermMode; ///< 100, 1k, 10k 
  Int_t fSampMode; ///< Unclear
  Int_t fExtTrigMode; ///< On or off
  Int_t fEventNumber; ///< Software event number
  Int_t fTargetEventNumber; ///< Event counter from Target??
  Int_t fNumPedEvents;
  UShort_t fBuffer[BUFFERSIZE];
  UShort_t fReadoutBuffer[BUFFERSIZE+2];
  Int_t fPedSubbedBuffer[NUM_CHANNELS][SAMPLES_PER_COL];
  Int_t fPedestalValues[NUM_CHANNELS][NUM_ROWS][NUM_COLS][SAMPLES_PER_COL];
  Float_t fVoltBuffer[NUM_CHANNELS][SAMPLES_PER_COL];

};


#endif //TARGET
