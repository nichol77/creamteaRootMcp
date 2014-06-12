void runMuonAnalysis(){
  gSystem->AddIncludePath("-I/home/creamtea/rootMcp/branches/multiusb/");
 
  gSystem->Load("libGraf.so");  
  gSystem->Load("libPhysics.so");  
  gSystem->Load("../libMcpTargetRoot.so");

  bool success = gSystem->CompileMacro("muonAnalysis.C","k");
  printf("Success = %d\n", success);
  muonAnalysis("~/CREAM_TEA/data/outputFile363.root"); //path of analysed data file as argument
}