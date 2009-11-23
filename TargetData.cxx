 #include "TargetData.h"
ClassImp(TargetData)
/////////////////////////////////////////////////////////
TargetData::TargetData(void)
{}
/////////////////////////////////////////////////////////
TargetData::~TargetData(void)
{}
/////////////////////////////////////////////////////////
TargetData::TargetData(unsigned short rawData[])
{
  for(int i=0;i<BUFFERSIZE;i++)
    RAW[i]=rawData[i];
  unpackData();
}

/////////////////////////////////////////////////////////
void TargetData::unpackData()
{  
  unsigned int RAW_TEMP[4];
  unsigned int RAW_ROVDD[4];
  unsigned int RAW_SCALER[4][3];
  int CNT = 1;
  int i,j,k;
  
  for(i=0; i<4; i++){
    MEM_ADDR_SPACE[i] = RAW[CNT];
	CNT++;    
    for(j=0; j<16; j++){
	  for(k=0; k<64; k++){
        DATA[i][j][k] = RAW[CNT];
		CNT++;
      }
	}
    for(j=0; j<3; j++){   
      RAW_SCALER[i][j] = RAW[CNT];
	  CNT++;
    }
    THRES[i] = RAW[CNT];
	CNT++;   
    RAW_ROVDD[i] = RAW[CNT];
	CNT++;   
    WBIAS[i] = RAW[CNT];
	CNT++;   
    FEEDBACK[i] = RAW[CNT];
	CNT++;   
    RAW_TEMP[i] = RAW[CNT];
	CNT++;   
  }	

  for(i=0; i<4; i++){
    temperature[i] = (float)RAW_TEMP[i]*0.03125;//degree C
    ROVDD[i] = (float)RAW_ROVDD[i]*2.5/4096;//volts
    for(j=0; j<3; j++){ 	
      SCALER[i][j] = (float)RAW_SCALER[i][j]/60.0;//Hz	
    }
    GetMEM_ADDR(MEM_ADDR_SPACE[i],
		ROW_LOC[i],
		COL_LOC[i],
		PIX_LOC[i],
		HIT_BIT[i]);
    GetFEEDBACK(FEEDBACK[i],
		SGN[i],
		PED_ROW_ADDR[i],
		PED_COL_ADDR[i],
		TERM[i],
		EN_PED[i]);		
  }
}
/////////////////////////////////////////////////////////
void TargetData::GetMEM_ADDR(unsigned int MEM_ADDR_SPACE, unsigned int &ROW_LOC, unsigned int &COL_LOC, unsigned int &PIX_LOC, unsigned int &HIT_BIT)
{
    const unsigned int MASK_ROW = 0x00000E00;
    ROW_LOC = MEM_ADDR_SPACE & MASK_ROW;
    ROW_LOC = ROW_LOC >> 9;
	
    const unsigned int MASK_COL = 0x000001F0;
    COL_LOC = MEM_ADDR_SPACE & MASK_COL;	
    PIX_LOC = COL_LOC;
    COL_LOC = COL_LOC >> 4;	

    const unsigned int MASK_HITBIT = 0x00008000;
    HIT_BIT = MEM_ADDR_SPACE & MASK_HITBIT;
    HIT_BIT = HIT_BIT >> 15;	
}
/////////////////////////////////////////////////////////
void TargetData::GetFEEDBACK(unsigned int FEEDBACK, unsigned int &SGN, unsigned int &PED_ROW_ADDR, unsigned int &PED_COL_ADDR, unsigned int &TERM, unsigned int &EN_PED)
{
    const unsigned int MASK_SGN = 0x00001000;
    SGN = FEEDBACK & MASK_SGN;
    SGN = SGN >> 12;
	
    const unsigned int MASK_ROW = 0x00000E00;
    PED_ROW_ADDR = FEEDBACK & MASK_ROW;   	
    PED_ROW_ADDR = PED_ROW_ADDR >> 9;	
	
    const unsigned int MASK_COL = 0x000001F0;	
    PED_COL_ADDR = FEEDBACK & MASK_COL;
	PED_COL_ADDR = PED_COL_ADDR >> 4;
	
    const unsigned int MASK_TERM = 0x0000000E;	
    TERM = FEEDBACK & MASK_TERM;
	TERM = TERM >> 1;
	
    const unsigned int MASK_EN_PED = 0x00000001;	
    EN_PED = FEEDBACK & MASK_EN_PED;
}
/////////////////////////////////////////////////////////
