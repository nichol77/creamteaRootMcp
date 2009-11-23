#ifndef TARGETDATA_H
#define TARGETDATA_H
#include "Defs.h"


class TargetData
{
 public:
  TargetData();
  TargetData(unsigned short rawData[BUFFERSIZE])
  ~TargetData();  
  void unpackData();

  unsigned int RAW[BUFFERSIZE]; 
  //extracted values 
  unsigned int MEM_ADDR_SPACE[NUM_TARGETS];
  unsigned int ROW_LOC[NUM_TARGETS];
  unsigned int COL_LOC[NUM_TARGETS];
  unsigned int PIX_LOC[NUM_TARGETS];
  unsigned int HIT_BIT[NUM_TARGETS];
  unsigned int DATA[NUM_TARGETS][NUM_CHANNELS][SAMPLES_PER_COL]; 
  float SCALER[NUM_TARGETS][3];//Hz
  unsigned int THRES[NUM_TARGETS];
  float ROVDD[3];//volts
  unsigned int WBIAS[NUM_TARGETS];
  unsigned int FEEDBACK[NUM_TARGETS];
  unsigned int SGN[NUM_TARGETS];  
  unsigned int PED_ROW_ADDR[NUM_TARGETS];  
  unsigned int PED_COL_ADDR[NUM_TARGETS];  
  unsigned int TERM[NUM_TARGETS];  
  unsigned int EN_PED[NUM_TARGETS];  
  float temperature[NUM_TARGETS];//degree C


 private:
  void GetMEM_ADDR(unsigned int MEM_ADDR_SPACE, unsigned int &ROW_LOC, unsigned int &COL_LOC, unsigned int &PIX_LOC, unsigned int &HIT_BIT);
  void GetFEEDBACK(unsigned int FEEDBACK, unsigned int &SGN, unsigned int &PED_ROW_ADDR, unsigned int &PED_COL_ADDR, unsigned int &TERM, unsigned int &EN_PED);

};

#endif /* TARGETDATA_H */
