#include <iostream>
#include <fstream>
#include "Defs.h"
#include "McpTarget.h"
#include "MultiRawTargetModules.h"
#include "MultiTargetModules.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TMultiGraph.h"

TGraph *getBoxCar(TGraph *grWave, Int_t halfWidth) ;
int getNiceColour(int index);
Int_t findNegPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp);
Int_t findPosPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp);

void pulseFinder(char *fileName) {
  McpTarget *myTarget = new McpTarget(1);
  //  myTarget->loadPedestal();
  MultiRawTargetModules *multiRawTargetDataPtr=0;
  MultiTargetModules *multiTargetDataPtr=0;
  TFile *offlineFile = new TFile(fileName);
  TTree *mcpTree = (TTree*) offlineFile->Get("mcpTree");
  if(!mcpTree) {
    std::cerr << "No input tree -- giving up\n";
    exit(0);
  }
  mcpTree->SetBranchAddress("target",&multiRawTargetDataPtr);
  Int_t numEntries=mcpTree->GetEntries();
  //  numEntries=100;
  std::cout << "Looping over " << numEntries << " entries.\n";


 

  for(int i=2;i<10;i++) {
    mcpTree->GetEntry(i);
    if(i%100==0) std::cerr << "*";
    if(multiTargetDataPtr) delete multiTargetDataPtr;
    multiTargetDataPtr=new MultiTargetModules(multiRawTargetDataPtr);
    myTarget->fillVoltageArray(multiTargetDataPtr);
   
    for(int module=0;module<4;module++) {
      for(int chan=0;chan<64;chan++) {
	TGraph *grTemp = multiTargetDataPtr->getChannel(module,chan);
	TGraph *grChan= getBoxCar(grTemp,3);
	//      grChan->Draw("alp");
	//    std::cout << grChan->GetMean(2) << "\n";
	Int_t peakSamp=0;
	Double_t peakArea=0,peakVal=0;
	Int_t numPeakSamps=findNegPeakAndArea(grChan,&peakArea,&peakVal,&peakSamp);
	//
	//      delete grTemp;
	// if(peakArea>150 && numPeakSamps>25) {
// 	  Double_t posPeakArea,posPeakVal;
// 	  Int_t posPeakSamp;
// 	  Int_t numPosPeakSamps=findPosPeakAndArea(grChan,&posPeakArea,&posPeakVal,&posPeakSamp);


// 	  if(posPeakArea>peakArea) {
// 	    delete grChan;
// 	    delete grTemp;
// 	    continue;
// 	  }

// 	  std::cout << i << "\t" << module << "\t" << chan << "\t" << numPeakSamps << "\t" << peakVal << "\t" << peakSamp << "\t" << peakArea << "\t" << grChan->GetRMS(2) << "\n";
// 	  //	  std::cout << "\t\t\t\t" << numPosPeakSamps << "\t" << posPeakSamp << "\t" << posPeakArea << "\t" << posPeakVal << "\n";

// 	  TCanvas *can = new TCanvas();
// 	  grChan->Draw("alp");
// 	  grTemp->SetMarkerColor(kRed);
// 	  grTemp->Draw("lp");
// 	}
// 	else {
// 	  delete grChan;
// 	  delete grTemp;
// 	}
      }    
    }
  }
}



int getNiceColour(int index)
{
    if(index>10) return index;
    Int_t niceColours[11]={kAzure+2,kRed+1,kGreen+1,kMagenta+1,kCyan+1,kOrange+2,kGreen-2,12,40,20,41};
    return niceColours[index];
}



TGraph *getBoxCar(TGraph *grWave, Int_t halfWidth) 
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

Int_t findNegPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp) 
{
  Double_t meanVal=gr->GetMean(2);
  Double_t *yVals = gr->GetY();
  Double_t minVal=meanVal;
  Int_t numSamps=gr->GetN();
  Int_t minSamp=-1;
  for(int i=0;i<numSamps;i++) {
    if(yVals[i]<minVal) {
      minVal=yVals[i];
      minSamp=i;
    }    
  }
  Int_t numPeakSamps=0;
  Double_t area=0;
  for(int i=minSamp;i>=0;i--) {
    if(yVals[i]<meanVal) {
      area+=-1*(yVals[i]-meanVal);
      numPeakSamps++;
    }
    else {
      break;
    }
  }
  for(int i=minSamp+1;i<numSamps;i++) {
    if(yVals[i]<meanVal) {
      area+=-1*(yVals[i]-meanVal);
      numPeakSamps++;
    }
    else {
      break;
    }
  }


  *peakArea=area;
  *peakVal=minVal;
  *peakSamp=minSamp;
  //  std::cout << minSamp << "\t" << minVal << "\t" << area << "\n";
  return numPeakSamps;
}


Int_t findPosPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp) 
{
  Double_t meanVal=gr->GetMean(2);
  Double_t *yVals = gr->GetY();
  Double_t maxVal=meanVal;
  Int_t numSamps=gr->GetN();
  Int_t maxSamp=-1;
  for(int i=0;i<numSamps;i++) {
    if(yVals[i]>maxVal) {
      maxVal=yVals[i];
      maxSamp=i;
    }    
  }
  Int_t numPeakSamps=0;
  Double_t area=0;
  for(int i=maxSamp;i>=0;i--) {
    if(yVals[i]>meanVal) {
      area+=(yVals[i]-meanVal);
      numPeakSamps++;
    }
    else {
      break;
    }
  }
  for(int i=maxSamp+1;i<numSamps;i++) {
    if(yVals[i]>meanVal) {
      area+=(yVals[i]-meanVal);
      numPeakSamps++;
    }
    else {
      break;
    }
  }


  *peakArea=area;
  *peakVal=maxVal;
  *peakSamp=maxSamp;
  return numPeakSamps;
}
