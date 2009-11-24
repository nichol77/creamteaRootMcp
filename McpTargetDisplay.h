//////////////////////////////////////////////////////////////////////////////
/////  McpTargetDisplay.h        McpTargetDisplay                              /////
/////                                                                    /////
/////  Description:                                                      /////
/////     This is a simple class to display data from the TARGET_eval    ///// 
/////     test board                                                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TARGETDISPLAY_H
#define TARGETDISPLAY_H

//Includes
#include "TChain.h"
//#include "McpTargetDisplayConventions.h"

#include "McpTarget.h" //The interface class
#include "TargetData.h" //The data class
#include "RawTargetData.h" //The data class

class TCanvas;
class TPad;
class TButton;
class TTreeIndex;
class TFile;
class TThread;

class McpTargetDisplay 
{
 public:
  
  McpTargetDisplay(int offlineMode=0,TFile *inputFile=0);
  ~McpTargetDisplay();
  
  static McpTargetDisplay* Instance();


  //Event playback mode
  void startEventPlaying();
  void stopEventPlaying();

  //Waveform event display stuff
  void setOfflineMode(TFile *inputFile);
  void startEventDisplay(); //these are 
  int displayNextEvent(); // all the
  void refreshEventDisplay(); // same
  void toggleView(Int_t view) {fView=view;}
  int getEventDisplayPlayMode() {return fInEventPlayMode;}
  void setEventDisplayPlayMode(int mode) { fInEventPlayMode=mode;}
  int getEventDisplayPlaySleep() {return fEventPlaySleepMs;}
  void drawEventButtons();

  McpTarget *getMcpTargetPointer() {return &(this->fTheMcpTarget);}

 protected:
   static McpTargetDisplay *fgInstance;  
   // protect against multiple instances

 private:
   TCanvas *fMcpTargetCanvas; //The main canvas
   TPad *fMcpTargetMainPad;
   TPad *fMcpTargetEventInfoPad;

   TButton *fWaveformButton;
   TButton *fPowerButton;

   Int_t fInEventPlayMode;
   Int_t fEventPlaySleepMs;
   Int_t fView; //1 is waveforms, 2 is FFTs

   McpTarget fTheMcpTarget;
   TargetData *fTheTargetDataPtr;
   RawTargetData *fTheRawTargetDataPtr;
   TFile *fTheOfflineFile;
   TTree *fTheOfflineTree;
   Int_t fOfflineMode;
   Long64_t fTheOfflineEntry;
};


#endif //TARGETDISPLAY_H
