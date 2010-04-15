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
class TPaveText;
class TSlider;

class McpTargetDisplay 
{
 public:
  
  McpTargetDisplay(int offlineMode=0,TFile *inputFile=0);
  ~McpTargetDisplay();
  
  static McpTargetDisplay* Instance();

  //Threshold stuff
  void updateThresholdFromSlider();
  void updateThreshold(UInt_t threshold);

  //Event playback mode
  void startEventPlaying();
  void stopEventPlaying();

  //Waveform event display stuff
  void setOfflineMode(TFile *inputFile);
  void startEventDisplay(); //these are 
  int displayNextEvent(); // all the
  void refreshEventDisplay(); // same
  void toggleView(Int_t view) {fView=view;}
  void toggleAutoscale();
  int getEventDisplayPlayMode() {return fInEventPlayMode;}
  void setEventDisplayPlayMode(int mode) { fInEventPlayMode=mode;}
  int getEventDisplayPlaySleep() {return fEventPlaySleepMs;}
  void drawEventButtons();
  void drawZoomButtons();
  void setFixedRange();

  McpTarget *getMcpTargetPointer() {return &(fTheMcpTarget);}

 protected:
   static McpTargetDisplay *fgInstance;  
   // protect against multiple instances

 private:
   TCanvas *fMcpTargetCanvas; //The main canvas
   TPad *fMcpTargetMainPad;
   TPad *fMcpTargetEventInfoPad;

   TPaveText *fFarLeftPave;
   TPaveText *fMidLeftPave;
   TPaveText *fMidMidPave;
   TPaveText *fMidRightPave;
   TPaveText *fFarRightPave;


   TButton *fWaveformButton;
   TButton *fPowerButton;

   TButton *fZoomButton;
   TSlider *fZoomSlider;
   TSlider *fThresholdSlider;

   Int_t fInEventPlayMode;
   Int_t fEventPlaySleepMs;
   Int_t fView; //1 is waveforms, 2 is FFTs
   Int_t fAutoScaleMode; //1 is autoscale, 0 is fixed scale
   Int_t fMinScale; 
   Int_t fMaxScale;
   Int_t fAbsMaxScale;

   McpTarget fTheMcpTarget;
   TargetData *fTheTargetDataPtr;
   RawTargetData *fTheRawTargetDataPtr;
   TFile *fTheOfflineFile;
   TTree *fTheOfflineTree;
   Int_t fOfflineMode;
   Long64_t fTheOfflineEntry;
};


#endif //TARGETDISPLAY_H
