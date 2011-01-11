 #include "MultiTargetModules.h"
ClassImp(MultiTargetModules)
/////////////////////////////////////////////////////////
MultiTargetModules::MultiTargetModules(void)
{}
/////////////////////////////////////////////////////////
MultiTargetModules::~MultiTargetModules(void)
{}

MultiTargetModules::MultiTargetModules(int numModules, unsigned short *buffer)
{
  numTargetModules=numModules;
  for(int i=0;i<numModules;i++) {
    targetData[i].setRawBuffer(&buffer[i*BUFFERSIZE]);
    targetData[i].unpackData();
  }

}

MultiTargetModules::MultiTargetModules(MultiRawTargetModules *raw)
{
   numTargetModules=raw->numTargetModules;
   for(int i=0;i<numTargetModules;i++) {
      targetData[i].setRawBuffer(raw->targetData[i].raw);
      targetData[i].unpackData();
   }
   
}
