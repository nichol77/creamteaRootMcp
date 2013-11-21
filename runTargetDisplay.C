gSystem->Reset();

void runTargetDisplay() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  //  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  //  gSystem->Load("libgsl.so");
  //  gSystem->Load("libusb.so");
  //  gSystem->Load("libfftw3.so");
  //  gSystem->Load("libMathMore.so");
  //  gSystem->Load("libRootFftwWrapper.so");   

  gSystem->Load("libGraf.so");  
  gSystem->Load("libPhysics.so");  
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
    ofstream RunNumFile2("theLatestRunNumber.txt");
    if(RunNumFile2) {
      RunNumFile2 << run << "\n";
    }
    RunNumFile2.close();
  }
  
  char outName[180];
  sprintf(outName,"data/outputFile%d.root",run);  
  McpTargetDisplay *displayPtr = new McpTargetDisplay();
  McpTarget *targetPtr = displayPtr->getMcpTargetPointer();
  targetPtr->openOutputFile(outName);
  std::cout << "setSoftTrigMode\n";
  targetPtr->setSoftTrigMode(0);
  std::cout << "setTrigThresh\n";
  //UInt_t threshArray[4]={2030,2100,2150,2030};
  //  UInt_t threshArray[4]={2030,2030,2010,2010};

  UInt_t globalThreshold=1750;
  UInt_t threshArray[4]={globalThreshold,globalThreshold,globalThreshold,globalThreshold};
  targetPtr->setTrigThresh(threshArray);
  std::cout << "setTrigPolarity\n";
  targetPtr->setTrigPolarity(0);
  std::cout << "setWbias\n";
  targetPtr->setWbias(800);  //800

  std::cout << "About to start display\n";
  displayPtr->startEventDisplay();  
  
}
