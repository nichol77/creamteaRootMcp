 #include "TargetData.h"
ClassImp(TargetData)
/////////////////////////////////////////////////////////
TargetData::TargetData(void)
:RawTargetData()
{}

TargetData::TargetData(RawTargetData *rawPtr)
  :RawTargetData(*rawPtr)
{
  unpackData();
}
/////////////////////////////////////////////////////////
TargetData::~TargetData(void)
{}
/////////////////////////////////////////////////////////
TargetData::TargetData(unsigned short rawData[])
  :RawTargetData(rawData)
{
  unpackData();
}

/////////////////////////////////////////////////////////
void TargetData::unpackData()
{  
  unsigned int raw_temp[NUM_TARGETS];
  unsigned int raw_rovdd[NUM_TARGETS];
  unsigned int raw_scaler[NUM_TARGETS][3];
  int count = 1;
  int chip,channel,samp;
  int j;

  for(chip=0; chip<NUM_TARGETS; chip++){
    memAddrSpace[chip] = raw[count];
    count++;    
    for(channel=0; channel<NUM_CHANNELS; channel++){
      for(samp=0; samp<SAMPLES_PER_COL; samp++){
        data[chip][channel][samp] = raw[count];
	count++;
      }
    }
    for(j=0; j<3; j++){   
      raw_scaler[chip][j] = raw[count];
	  count++;
    }
    thresh[chip] = raw[count];
	count++;   
    raw_rovdd[chip] = raw[count];
	count++;   
    wbias[chip] = raw[count];
	count++;   
    feedback[chip] = raw[count];
	count++;   
    raw_temp[chip] = raw[count];
	count++;   
  }	

  for(chip=0; chip<NUM_TARGETS; chip++){
    temperature[chip] = (float)raw_temp[chip]*0.03125;//degree C
    rovdd[chip] = (float)raw_rovdd[chip]*2.5/4096;//volts
    for(j=0; j<3; j++){ 	
      scaler[chip][j] = (float)raw_scaler[chip][j]/60.0;//Hz	
    }
  }
  unpackMemAddrSpace();
  unpackFeedback();
}
/////////////////////////////////////////////////////////
void TargetData::unpackMemAddrSpace()
{
    const unsigned int MASK_ROW = 0x00000E00;
    const unsigned int MASK_COL = 0x000001F0;
    const unsigned int MASK_HITBIT = 0x00008000;
    for(int chip=0;chip<NUM_TARGETS;chip++) {
      rowLoc[chip] = memAddrSpace[chip] & MASK_ROW;
      rowLoc[chip] = rowLoc[chip] >> 9;
    
      colLoc[chip] = memAddrSpace[chip] & MASK_COL;	
      pixLoc[chip] = colLoc[chip];
      colLoc[chip] = colLoc[chip] >> 4;	

      hitBit[chip] = memAddrSpace[chip] & MASK_HITBIT;
      hitBit[chip] = hitBit[chip] >> 15;	
    }
}
/////////////////////////////////////////////////////////
void TargetData::unpackFeedback()
{
    const unsigned int MASK_SGN = 0x00001000;
    const unsigned int MASK_ROW = 0x00000E00;
    const unsigned int MASK_COL = 0x000001F0;
    const unsigned int MASK_TERM = 0x0000000E;	
    const unsigned int MASK_EN_PED = 0x00000001;
    for(int chip=0;chip<NUM_TARGETS;chip++) {
      sign[chip] = feedback[chip] & MASK_SGN;
      sign[chip] = sign[chip] >> 12;
    
      pedRowAddr[chip] = feedback[chip] & MASK_ROW;   	
      pedRowAddr[chip] = pedRowAddr[chip] >> 9;	
      
      pedColAddr[chip] = feedback[chip] & MASK_COL;
      pedColAddr[chip] = pedColAddr[chip] >> 4;
      
      term[chip] = feedback[chip] & MASK_TERM;
      term[chip] = term[chip] >> 1;
      
      enPed[chip] = feedback[chip] & MASK_EN_PED;
    }
}
/////////////////////////////////////////////////////////


TGraph *TargetData::getChannel(int chip, int channel)
{
  if(chip<0 || chip>=NUM_TARGETS) return NULL;
  if(channel<0 || channel>=NUM_CHANNELS) return NULL;
  
  Double_t timeVals[SAMPLES_PER_COL]={0};
  Double_t voltVals[SAMPLES_PER_COL]={0};
  for(int samp=0;samp<SAMPLES_PER_COL;samp++) {
     timeVals[samp]=Double_t(samp); //Should multiple by delta t here
     voltVals[samp]=fVoltBuffer[chip][channel][samp];
  }
  TGraph *gr = new TGraph(SAMPLES_PER_COL,timeVals,voltVals);
  return gr;
}
