gSystem->Reset();

void runTargetDisplay() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

gSystem->Load("libgsl.so");
  gSystem->Load("libusb.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libGraf.so");  
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");   
  gSystem->Load("libMcpTargetRoot.so");

//   TChain *fred=0; //Will this work?
  Int_t run=0;
  {
    ifstream RunNumFile("theLatestRunNumber.txt");
    if(RunNumFile) {
      RunNumFile >> run;
    }
    RunNumFile.close();
  }
  run++;
  {
    ofstream RunNumFile("theLatestRunNumber.txt");
    if(RunNumFile) {
      RunNumFile << run;
    }
    RunNumFile.close();
  }
  
  char outName[180];
  sprintf(outName,"data/outputFile%d.root",run);  
  McpTargetDisplay *displayPtr = new McpTargetDisplay();
  McpTarget *targetPtr = displayPtr->getMcpTargetPointer();
  targetPtr->openOutputFile(outName);
  targetPtr->setSoftTrigMode(0);
  displayPtr->startEventDisplay();
  
}
