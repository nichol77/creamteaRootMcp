gSystem->Reset();

void createOfflineFile() { 

//   gSystem->Load("libusb.so");
//   gSystem->Load("libfftw3.so");
//   gSystem->Load("libMathMore.so");
//   gSystem->Load("libRootFftwWrapper.so");   
  gSystem->Load("libGraf.so");
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libMcpTargetRoot.so");

//   TChain *fred=0; //Will this work?
  std::cout << "Usage\n";
  std::cout << "Create an McpTarget object\n";
  std::cout <<"\tMcpTarget *myTarget = new McpTarget();\n";
  std::cout << "Do something\n";
  McpTarget *myTarget = new McpTarget();
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
      RunNumFile2 << run;
    }
    RunNumFile2.close();
  }    
  std::cout << "Starting run " << run << "\n";
  char outName[180];
  sprintf(outName,"data/outputFile%d.root",run);  
  myTarget->openOutputFile(outName);
  TStopwatch stopy;
  stopy.Start();

  Int_t numEvents=1000;

  myTarget->setSoftTrigMode(0);
  for(int i=0;i<numEvents;i++) {
    //    if(i%100==0) 
    std::cerr << "*";
    myTarget->readEvent();
  }
  stopy.Stop();
  std::cerr << "\n";
  std::cout << "Took " << numEvents << " events in " << stopy.RealTime()
	    << "s at " << numEvents/stopy.RealTime() << "Hz\n";
  myTarget->saveOutputFile();
}
