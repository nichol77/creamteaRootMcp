#ifndef MULTITARGETMODULES_H
#define MULTITARGETMODULES_H
#include "Defs.h"
#include "TObject.h"
#include "TGraph.h"
#include "TargetData.h"
#include "MultiRawTargetModules.h"

class MultiTargetModules: public TObject
{
 public:
  MultiTargetModules();
  MultiTargetModules(MultiRawTargetModules *raw);
  MultiTargetModules(int numModules, unsigned short *buffer);
  ~MultiTargetModules();  
  int getNumModules() {return numTargetModules;}
  
  TGraph *getChannel(int module, int chanInd) {
     if(module>=0 && module<numTargetModules) return targetData[module].getChannel(chanInd); return NULL;}
  TGraph *getChannel(int module, int chip, int chan) {if(module>=0 && module<numTargetModules) return targetData[module].getChannel(chip,chan); return NULL;}

  int numTargetModules;
  TargetData targetData[MAX_TARGET_MODULES];
  
  ClassDef(MultiTargetModules,3); //Need to update to 2 when switch to 64
};

#endif /* MULTITARGETMODULES_H */
