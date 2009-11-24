gSystem->Reset();

void createOfflineFile() { 

  gSystem->Load("libusb.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libRootFftwWrapper.so");   
  gSystem->Load("libMcpTargetRoot.so");

//   TChain *fred=0; //Will this work?
  std::cout << "Usage\n";
  std::cout << "Create an McpTarget object\n";
  std::cout <<"\tMcpTarget *myTarget = new McpTarget();\n";
  std::cout << "Do something\n";
  McpTarget *myTarget = new McpTarget();
  myTarget->openOutputFile("outputFile.root");
  for(int i=0;i<100;i++) {
    std::cerr << "*";
    myTarget->sendSoftTrig();
    myTarget->readEvent();
  }
  std::cerr << "\n";
  myTarget->saveOutputFile();
}
