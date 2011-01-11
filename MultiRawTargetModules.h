#ifndef MULTIRAWTARGETMODULES_H
#define MULTIRAWTARGETMODULES_H
#include "Defs.h"
#include "TObject.h"
#include "RawTargetData.h"

class MultiRawTargetModules: public TObject
{
 public:
  MultiRawTargetModules();
  MultiRawTargetModules(int numModules, unsigned short *buffer);
  ~MultiRawTargetModules();  

  int numTargetModules;
  RawTargetData targetData[MAX_TARGET_MODULES];
  
  ClassDef(MultiRawTargetModules,3); //Need to update to 2 when switch to 64
};

#endif /* MULTIRAWTARGETMODULES_H */
