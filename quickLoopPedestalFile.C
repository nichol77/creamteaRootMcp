gSystem->Reset();

void quickLoopPedestalFile() {
  gSystem->AddIncludePath("-I${RJN_UTIL_INSTALL_DIR}/include");

  gSystem->Load("libGraf.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libMcpTargetRoot.so");

  McpTarget *myTarget = new McpTarget(1);
  RawTargetData *rawTargetDataPtr=0;
  TargetData *targetDataPtr=0;
  //  TFile *offlineFile = new TFile("data/outputFile37.root");
  //  TFile *offlineFile = new TFile("data/outputFile46.root");
  //  TFile *offlineFile = new TFile("data/outputFile131.root");
  TFile *pedFile = new TFile("pedestalStuff/pedFile.root");
  //  TFile *pedFile = new TFile("data/outputFile17.root");
  TTree *pedTree = (TTree*) pedFile->Get("pedTree");
  if(!pedTree) {
    std::cerr << "No input tree -- giving up\n";
    exit(0);
  }
  pedTree->SetBranchAddress("target",&rawTargetDataPtr);
  Int_t numEntries=pedTree->GetEntries();
  numEntries=1000;


  Int_t minSamp;
  Double_t peakVal;
  Double_t peakValOther;
  Double_t meanVal;
  Int_t row,col;
  Int_t lastRow=-1;
  Int_t lastCol=-1;
  Int_t hit;
  Int_t adcs[512];
  Int_t pedSubbed[512];
  TFile *outFile = new TFile("pedHistFile.root","RECREATE");

  TH1F *histPed[512];
  TH1F *histAllPed[512];
  char histName[180];
  char histTitle[180];
  for(int samp=0;samp<512;samp++) {
    sprintf(histName,"histRaw_%d",samp);
    sprintf(histTitle,"histRaw_%d",samp);
    histPed[samp]= new TH1F(histName,histTitle,2000,-0.5,1999.5);
    sprintf(histName,"histAll_%d",samp);
    sprintf(histTitle,"histAll_%d",samp);
    histAllPed[samp]= new TH1F(histName,histTitle,2000,-0.5,1999.5);
  }
    
  

  for(int i=0;i<numEntries;i++) {
    pedTree->GetEntry(i);
    if(i%100==0) std::cerr << "*";
    if(targetDataPtr) delete targetDataPtr;
    targetDataPtr=new TargetData(rawTargetDataPtr);

    
    if(targetDataPtr->rowLoc[0]==0 ) {
      if(targetDataPtr->colLoc[0]==0) {
	for(int samp=0;samp<512;samp++) {
	  histPed[samp]->Fill(targetDataPtr->data[0][0][samp]);
	}
      }
      for(int samp=0;samp<512;samp++) {
	int cell=(targetDataPtr->pixLoc[0]+samp)%512;
	histAllPed[cell]->Fill(targetDataPtr->data[0][0][samp]);
      }
    }
    //    std::cout << targetDataPtr->rowLoc[0] << "\t" << targetDataPtr->colLoc[0] << "\t" << targetDataPtr->pixLoc[0] << "\n";
  }
  
  
  std::cerr << "\n";
  outFile->Write();
}

void getNegPeakAndTime(TGraph *grIn, Int_t &sampNum, Double_t &peakVal)
{
  Int_t numSamps=grIn->GetN();
  Double_t *xVals=grIn->GetX();
  Double_t *yVals=grIn->GetY();
  
  Double_t minVal=10000;
  Int_t minBin=-1;

  for(int i=0;i<numSamps;i++) {
    if(yVals[i] < minVal ) {
      minVal=yVals[i];
      minBin=i;
    }
  }
  sampNum=minBin;
  peakVal=minVal;
}
