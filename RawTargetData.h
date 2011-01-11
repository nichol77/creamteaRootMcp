#ifndef RAWTARGETDATA_H
#define RAWTARGETDATA_H
#include "Defs.h"
#include "TObject.h"


class RawTargetData: public TObject
{
 public:
  RawTargetData();
  RawTargetData(unsigned short rawData[BUFFERSIZE]);
  ~RawTargetData();  
  void setRawBuffer(unsigned short rawData[BUFFERSIZE]);
  void setRawBuffer(unsigned int rawData[BUFFERSIZE]);

  unsigned int raw[BUFFERSIZE]; 
  ClassDef(RawTargetData,3); //Need to update to 2 when switch to 64
};

#endif /* RAWTARGETDATA_H */
