gSystem->Reset();

void loadTargetLibs() { 

  gSystem->Load("libGraf.so");
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
  
}
