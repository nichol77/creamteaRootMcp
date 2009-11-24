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
  unsigned int raw_temp[4];
  unsigned int raw_rovdd[4];
  unsigned int raw_scaler[4][3];
  int count = 1;
  int i,j,k;
  
  for(i=0; i<4; i++){
    memAddrSpace[i] = raw[count];
	count++;    
    for(j=0; j<16; j++){
	  for(k=0; k<64; k++){
        data[i][j][k] = raw[count];
		count++;
      }
	}
    for(j=0; j<3; j++){   
      raw_scaler[i][j] = raw[count];
	  count++;
    }
    thresh[i] = raw[count];
	count++;   
    raw_rovdd[i] = raw[count];
	count++;   
    wbias[i] = raw[count];
	count++;   
    feedback[i] = raw[count];
	count++;   
    raw_temp[i] = raw[count];
	count++;   
  }	

  for(i=0; i<4; i++){
    temperature[i] = (float)raw_temp[i]*0.03125;//degree C
    rovdd[i] = (float)raw_rovdd[i]*2.5/4096;//volts
    for(j=0; j<3; j++){ 	
      scaler[i][j] = (float)raw_scaler[i][j]/60.0;//Hz	
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
    for(int i=0;i<NUM_TARGETS;i++) {
      rowLoc[i] = memAddrSpace[i] & MASK_ROW;
      rowLoc[i] = rowLoc[i] >> 9;
    
      colLoc[i] = memAddrSpace[i] & MASK_COL;	
      pixLoc[i] = colLoc[i];
      colLoc[i] = colLoc[i] >> 4;	

      hitBit[i] = memAddrSpace[i] & MASK_HITBIT;
      hitBit[i] = hitBit[i] >> 15;	
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
    for(int i=0;i<NUM_TARGETS;i++) {
      sign[i] = feedback[i] & MASK_SGN;
      sign[i] = sign[i] >> 12;
    
      pedRowAddr[i] = feedback[i] & MASK_ROW;   	
      pedRowAddr[i] = pedRowAddr[i] >> 9;	
      
      pedColAddr[i] = feedback[i] & MASK_COL;
      pedColAddr[i] = pedColAddr[i] >> 4;
      
      term[i] = feedback[i] & MASK_TERM;
      term[i] = term[i] >> 1;
      
      enPed[i] = feedback[i] & MASK_EN_PED;
    }
}
/////////////////////////////////////////////////////////
