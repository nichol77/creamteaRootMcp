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

  unsigned int raw[BUFFERSIZE]; 
  ClassDef(RawTargetData,1);
};

#endif /* RAWTARGETDATA_H */
