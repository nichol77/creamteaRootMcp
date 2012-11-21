#include "CreamTeaPmtHit.h"
#include "TGraph.h"


ClassImp(CreamTeaPmtHit);



CreamTeaPmtHit::CreamTeaPmtHit() 
{
  //Default constructor
  chanIndex=-1;
  pulseArea=0;
  pulseVal=0;
  numPulseSamps=0;
  peakSamp=0;
}

CreamTeaPmtHit::CreamTeaPmtHit(TGraph *gr,Int_t module, Int_t chip, Int_t chan)
{
  //Assignment constructor
  chanIndex=getIndex(module,chip,chan);
  TGraph *grBox=getBoxCar(gr,3);
  fillHitInformation(grBox);
  delete grBox;
}

CreamTeaPmtHit::~CreamTeaPmtHit()
{
  //Destructor
}


void CreamTeaPmtHit::fillHitInformation(TGraph *gr) 
{
  Double_t meanVal=gr->GetMean(2);
  Double_t *yVals = gr->GetY();
  pulseVal=meanVal;
  Int_t numSamps=gr->GetN();
  peakSamp=-1;
  for(int i=0;i<numSamps;i++) {
    if(yVals[i]<pulseVal) {
      pulseVal=yVals[i];
      peakSamp=i;
    }    
  }
  numPulseSamps=0;
  pulseArea=0;
  if(peakSamp>-1) {
    for(int i=peakSamp;i>=0;i--) {
      if(yVals[i]<meanVal) {
	pulseArea+=-1*(yVals[i]-meanVal);
	numPulseSamps++;
    }
      else {
	break;
      }
    }
    for(int i=peakSamp+1;i<numSamps;i++) {
      if(yVals[i]<meanVal) {
	pulseArea+=-1*(yVals[i]-meanVal);
	numPulseSamps++;
      }
      else {
	break;
      }
    }    
  }

}

TGraph *CreamTeaPmtHit::getBoxCar(TGraph *grWave, Int_t halfWidth) 
{
  //Just do this the lazy way for now
  Double_t *inY = grWave->GetY();
  Double_t *inX = grWave->GetX();
  Int_t length=grWave->GetN();
  Double_t *smoothY = new Double_t[length];
  for(int i=0;i<length;i++) {
    smoothY[i]=0;
    if(i<halfWidth || length-i<=halfWidth) {
      int countVals=0;
      for(int j=i-halfWidth;j<=i+halfWidth;j++) {
	if(j>=0 && j<length) {
	  smoothY[i]+=inY[j];
	  countVals++;
	}
      }
      //      cout << i << "\t" << countVals << endl;
      smoothY[i]/=countVals;
    }
    else {
      for(int j=i-halfWidth;j<=i+halfWidth;j++) {
	smoothY[i]+=inY[j];
      }
      smoothY[i]/=1+2*halfWidth;
    }      
  }
  TGraph *grSmooth = new TGraph(length,inX,smoothY);
  delete [] smoothY;
  return grSmooth;
  
}
