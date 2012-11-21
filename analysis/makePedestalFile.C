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

int getNiceColour(int index);

void makePedestalFile(char *fileName) {
  //  McpTarget *myTarget = new McpTarget(1);
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

  Int_t adcVal;
  Int_t mcp,target;
  Int_t row,col,cell,chan,samp;

    
  Float_t sampNumber[SAMPLES_PER_COL];
  Float_t *mean[MAX_TARGET_MODULES][NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS]={{{{{0}}}}};
  Float_t *rms[MAX_TARGET_MODULES][NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS]={{{{{0}}}}};
  Float_t *meanSq[MAX_TARGET_MODULES][NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS]={{{{{0}}}}};
  Int_t *numEnts[MAX_TARGET_MODULES][NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS]={{{{{0}}}}};
  
  
  for(mcp=0;mcp<MAX_TARGET_MODULES;mcp++) {
    for(target=0;target<NUM_TARGETS;target++) {
      for(chan=0;chan<NUM_CHANNELS;chan++) {
	for(row=0;row<NUM_ROWS;row++) {
	  for(col=0;col<NUM_COLS;col++) {
	    mean[mcp][target][chan][row][col] = new Float_t[SAMPLES_PER_COL];
	    meanSq[mcp][target][chan][row][col] = new Float_t[SAMPLES_PER_COL];
	    rms[mcp][target][chan][row][col] = new Float_t[SAMPLES_PER_COL];
	    numEnts[mcp][target][chan][row][col] = new Int_t[SAMPLES_PER_COL];
	  }
	}
      }
    }
  }


  for(int i=0;i<numEntries;i++) {
    mcpTree->GetEntry(i);
    if(i%100==0) std::cerr << "*";
    if(multiTargetDataPtr) delete multiTargetDataPtr;
    multiTargetDataPtr=new MultiTargetModules(multiRawTargetDataPtr);

    for(mcp=0;mcp<MAX_TARGET_MODULES;mcp++) {
      for(target=0;target<NUM_TARGETS;target++) {
	row=multiTargetDataPtr->targetData[mcp].rowLoc[target];
	col=multiTargetDataPtr->targetData[mcp].colLoc[target];
	for(samp=0;samp<SAMPLES_PER_COL;samp++) {
	  for(chan=0;chan<NUM_CHANNELS;chan++) {	      
	    adcVal=multiTargetDataPtr->targetData[mcp].data[target][chan][samp];
	    if(adcVal>0 && adcVal<4095) {
	      numEnts[mcp][target][chan][row][col][samp]++;
	      mean[mcp][target][chan][row][col][samp]+=adcVal;
	      meanSq[mcp][target][chan][row][col][samp]+=adcVal*adcVal;
	    }
	  }
	}
      }
    }
  }
    //    std::cout << multiTargetDataPtr->targetData[0].rowLoc[0] << "\t" << multiTargetDataPtr->targetData[0].colLoc[0] << "\t" << multiTargetDataPtr->targetData[0].pixLoc[0] << "\n";

  
  
  std::cerr << "\n";
  
  Int_t cellEnt;
  Float_t cellMean,cellRMS;
  TFile *outFile = new TFile("histFile.root","RECREATE");
  TTree *outTree = new TTree("outTree","outTree");
  outTree->Branch("mcp",&mcp,"mcp/I");
  outTree->Branch("target",&target,"target/I");
  outTree->Branch("chan",&chan,"chan/I");
  outTree->Branch("row",&row,"row/I");
  outTree->Branch("col",&col,"col/I");
  outTree->Branch("samp",&samp,"samp/I");
  outTree->Branch("cellMean",&cellMean,"cellMean/F");
  outTree->Branch("cellRMS",&cellRMS,"cellRMS/F");
  outTree->Branch("cellEnt",&cellEnt,"cellEnt/I");

  for(mcp=0;mcp<MAX_TARGET_MODULES;mcp++) {
    for(target=0;target<NUM_TARGETS;target++) {
      for(chan=0;chan<NUM_CHANNELS;chan++) {	  
	for(row=0;row<NUM_ROWS;row++) {	  
	for(col=0;col<NUM_COLS;col++) {
	  for(samp=0;samp<SAMPLES_PER_COL;samp++) {
	    sampNumber[samp]=samp;
	    if(numEnts[mcp][target][chan][row][col][samp]>0) {
	      mean[mcp][target][chan][row][col][samp]/=numEnts[mcp][target][chan][row][col][samp];
	      meanSq[mcp][target][chan][row][col][samp]/=numEnts[mcp][target][chan][row][col][samp];
	      rms[mcp][target][chan][row][col][samp]=TMath::Sqrt(meanSq[mcp][target][chan][row][col][samp]-mean[mcp][target][chan][row][col][samp]*mean[mcp][target][chan][row][col][samp]);

	      cellEnt=numEnts[mcp][target][chan][row][col][samp];
	      cellMean=mean[mcp][target][chan][row][col][samp];
	      cellRMS=rms[mcp][target][chan][row][col][samp];
	      outTree->Fill();
	      //   std::cout << mcp << "\t" << target << "\t" << chan << "\t" << row << "\t" << col << "\t" << samp << "\t" << numEnts[mcp][target][chan][row][col][samp] << "\t" << mean[mcp][target][chan][row][col][samp] << "\t" << meanSq[mcp][target][chan][row][col][samp] << "\t" << rms[mcp][target][chan][row][col][samp]  << "\n" ;
	      
	    }
	  }
	}
      }
    }
  }
  }


  outTree->AutoSave();


  std::ofstream pedFile ("pedFile.txt");
  for(mcp=0;mcp<MAX_TARGET_MODULES;mcp++) {
    for(target=0;target<NUM_TARGETS;target++) {
      for(chan=0;chan<NUM_CHANNELS;chan++) {	  
	for(row=0;row<NUM_ROWS;row++) {	  
	  for(col=0;col<NUM_COLS;col++) {
	    for(samp=0;samp<SAMPLES_PER_COL;samp++) {
	      pedFile << TMath::Nint(mean[mcp][target][chan][row][col][samp]) << "\t";
	    }
	    pedFile << "\n";
	  }
	}
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
