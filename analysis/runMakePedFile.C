gSystem->Reset();

void runMakePedFile() {
  
  gSystem->AddIncludePath("-I/home/creamtea/rootMcp/branches/multiusb/");
 
  gSystem->Load("libGraf.so");  
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libMcpTargetRoot.so");

  gSystem->CompileMacro("makePedestalFile.C","k");
  makePedestalFile("../data/outputFile110.root");

}
