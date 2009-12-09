gSystem->Reset();

void runOfflineTargetDisplay() {
  gSystem->AddIncludePath("-I${RJN_UTIL_INSTALL_DIR}/include");

  gSystem->Load("libusb.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libgsl.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");   
  gSystem->Load("libMcpTargetRoot.so");

//   TChain *fred=0; //Will this work?
  
//  TFile *offlineFile = new TFile("outFiles/outputFile_251109.root");
//  TFile *offlineFile = new TFile("outFiles/outputFile_091209.root");
//  TFile *offlineFile = new TFile("outFiles/outputFile1k.root");
  TFile *offlineFile = new TFile("outFiles/outputFile1k.root");
  McpTargetDisplay *targetPtr = new McpTargetDisplay(1,offlineFile);
  targetPtr->startEventDisplay();
  
}
