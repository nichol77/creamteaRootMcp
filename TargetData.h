#ifndef TARGETDATA_H
#define TARGETDATA_H
#include "Defs.h"
#include "TObject.h"
#include "RawTargetData.h"
#include "TGraph.h"


class TargetData: public RawTargetData
{
 public:
  TargetData();
  TargetData(RawTargetData *rawData);
  TargetData(unsigned short rawData[BUFFERSIZE]);
  ~TargetData();  
  void unpackData();

  //extracted values 
  unsigned int memAddrSpace [NUM_TARGETS];
  unsigned int rowLoc[NUM_TARGETS];
  unsigned int colLoc[NUM_TARGETS];
  unsigned int pixLoc[NUM_TARGETS];
  unsigned int hitBit[NUM_TARGETS];
  unsigned int data[NUM_TARGETS][NUM_CHANNELS][SAMPLES_PER_COL]; 
  float scaler[NUM_TARGETS][3];//Hz
  unsigned int thresh[NUM_TARGETS];
  float rovdd[NUM_TARGETS];//volts
  unsigned int wbias[NUM_TARGETS];
  unsigned int feedback[NUM_TARGETS];
  unsigned int sign[NUM_TARGETS];  
  unsigned int pedRowAddr[NUM_TARGETS];  
  unsigned int pedColAddr[NUM_TARGETS];  
  unsigned int term[NUM_TARGETS];  
  unsigned int enPed[NUM_TARGETS];  
  float temperature[NUM_TARGETS];//degree C

  //This is ridiculous I will RSN (real soon now) move to a calibrated and uncalibrated system, but for now I'll be lazy;
  Float_t fVoltBuffer[NUM_TARGETS][NUM_CHANNELS][SAMPLES_PER_COL];
  
  TGraph *getChannel(Int_t chanInd) 
  { 
    return getChannel(chanInd/NUM_CHANNELS,
		      chanInd%NUM_CHANNELS);
  }
  TGraph *getChannel(Int_t chip, Int_t channel);
 private:
  void unpackMemAddrSpace();
  void unpackFeedback();
  ClassDef(TargetData,2);
};

#endif /* TARGETDATA_H */
