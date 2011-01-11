 #include "MultiRawTargetModules.h"
ClassImp(MultiRawTargetModules)
/////////////////////////////////////////////////////////
MultiRawTargetModules::MultiRawTargetModules(void)
{}
/////////////////////////////////////////////////////////
MultiRawTargetModules::~MultiRawTargetModules(void)
{}

MultiRawTargetModules::MultiRawTargetModules(int numModules, unsigned short *buffer)
{
  numTargetModules=numModules;
  for(int i=0;i<numModules;i++) {
    targetData[i].setRawBuffer(&buffer[i*BUFFERSIZE]);
  }

}

