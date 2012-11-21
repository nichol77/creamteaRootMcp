gSystem->Reset();

void runPulseFinder() {
  
  gSystem->AddIncludePath("-I/home/creamtea/rootMcp/branches/multiusb/");
 
  gSystem->Load("libGraf.so");  
  gSystem->Load("libPhysics.so");  
  gSystem->Load("libMcpTargetRoot.so");

  gSystem->CompileMacro("pulseFinder.C","k");
  pulseFinder("../data/outputFile109.root");

}
