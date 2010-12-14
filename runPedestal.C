gSystem->Reset();

void runPedestal() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  //  gSystem->Load("libusb.so");
  //  gSystem->Load("libfftw3.so");
  gSystem->Load("libGraf.so");
  // gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  //  gSystem->Load("libRootFftwWrapper.so");   
  gSystem->Load("libMcpTargetRoot.so");

//   TChain *fred=0; //Will this work?
   McpTarget *myTarget = new McpTarget();
   myTarget->setNumPedEvents(100);
   myTarget->generatePedestals();
  
}
