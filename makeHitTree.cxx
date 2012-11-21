#include <iostream>
#include <fstream>
#include "Defs.h"
#include "McpTarget.h"
#include "MultiRawTargetModules.h"
#include "MultiTargetModules.h"
#include "CreamTeaHitEvent.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TMultiGraph.h"


//Int_t findNegPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp);
//Int_t findPosPeakAndArea(TGraph *gr, Double_t *peakArea, Double_t *peakVal,  Int_t *peakSamp);

void usage(char **argv) {
  std::cerr << "Usage\n"
	    << argv[0] << "\t<input root file> <output root file> <run number>\n";
}


int main(int argc, char **argv) {

  //Do something
  if(argc<4) {
    usage(argv);
    return -1;
  }
  
  TFile *fpIn = TFile::Open(argv[1]);
  if(!fpIn) {
    std::cerr << "Couldn't open " << argv[1] << "\n";
    return -1;
  }
  Int_t run=atoi(argv[3]);
  
  MultiRawTargetModules *multiRawTargetDataPtr=0;
  MultiTargetModules *multiTargetDataPtr=0;
  TTree *inTree = (TTree*) fpIn->Get("mcpTree");
  if(!inTree) {
    std::cerr << "Can't find input tree -- giving up\n";
    return -1;
  }
  inTree->SetBranchAddress("target",&multiRawTargetDataPtr);
  Long64_t numEntries = inTree->GetEntries();
  if(numEntries<=0) {
    std::cerr << "No entries in input tree --giving up\n";
    return -1;
  }

  TFile *fpOut  = new TFile(argv[2],"RECREATE");
  if(!fpOut) {
    std::cerr << "Can not open output file " << argv[2] << "\n";
    return -1;
  }

  CreamTeaHitEvent *hitEventPtr=0;
  TTree *hitTree = new TTree("hitTree","Tree of CREAM TEA PMT Hits");
  hitTree->Branch("event","CreamTeaHitEvent",&hitEventPtr,16000,99);

  McpTarget *myTarget = new McpTarget(1);


  Long64_t starEvery=numEntries/80;
  if(starEvery==0) starEvery++;

  for(Long64_t i=0;i<numEntries;i++) {
    inTree->GetEntry(i);
    if(i%starEvery==0) std::cerr << "*";
    if(multiTargetDataPtr) delete multiTargetDataPtr;
    multiTargetDataPtr = new MultiTargetModules(multiRawTargetDataPtr);
    myTarget->fillVoltageArray(multiTargetDataPtr);
    
    if(hitEventPtr) delete hitEventPtr;
    hitEventPtr = new CreamTeaHitEvent(multiTargetDataPtr,0,0,run,i);
    hitTree->Fill();
  }
  std::cerr << "\n";
  hitTree->AutoSave();

  return 1;
}
