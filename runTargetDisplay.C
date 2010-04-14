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

  McpTargetDisplay *displayPtr = new McpTargetDisplay();
  McpTarget *targetPtr = displayPtr->getMcpTargetPointer();
  targetPtr->setSoftTrigMode(0);
  displayPtr->startEventDisplay();
  
}
