 #include "RawTargetData.h"
ClassImp(RawTargetData)
/////////////////////////////////////////////////////////
RawTargetData::RawTargetData(void)
{}
/////////////////////////////////////////////////////////
RawTargetData::~RawTargetData(void)
{}
/////////////////////////////////////////////////////////
RawTargetData::RawTargetData(unsigned short rawData[])
{
  for(int i=0;i<BUFFERSIZE;i++)
    raw[i]=rawData[i];
}