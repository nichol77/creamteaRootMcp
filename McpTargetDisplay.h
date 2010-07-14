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
#include "MultiTargetModules.h" //The data class
#include "MultiRawTargetModules.h" //The data class

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
  int displayPreviousEvent(); // all the
  int displayThisEntry(int entry) {fTheOfflineEntry=entry; return displayNextEvent();}
  void refreshEventDisplay(); // same
  void toggleView(Int_t view) {fView=view;}
  void toggleModule();
  void toggleAutoscale();
  int getEventDisplayPlayMode() {return fInEventPlayMode;}
  void setEventDisplayPlayMode(int mode) { fInEventPlayMode=mode;}
  int getEventDisplayPlaySleep() {return fEventPlaySleepMs;}
  void saveTree() { fTheMcpTarget.saveOutputFile();}
  void drawEventButtons();
  void drawZoomButtons();
  void setFixedRange();
  void setFixedRange(Double_t minVal,Double_t maxVal);

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

   TButton *fModuleButton;


   TButton *fZoomButton;
   TSlider *fZoomSlider;
   TSlider *fThresholdSlider;

   Int_t fInEventPlayMode;
   Int_t fEventPlaySleepMs;
   Int_t fView; //1 is waveforms, 2 is FFTs
   Int_t fAutoScaleMode; //1 is autoscale, 0 is fixed scale
   Double_t fMinScale; 
   Double_t fMaxScale;
   Int_t fAbsMaxScale;
   Int_t fWhichModule; //0-3

   McpTarget fTheMcpTarget;
   MultiTargetModules *fTheMultiPtr;
   MultiRawTargetModules *fTheMultiRawTargetModulesPtr;
   TFile *fTheOfflineFile;
   TTree *fTheOfflineTree;
   Int_t fOfflineMode;
   Long64_t fTheOfflineEntry;
};


#endif //TARGETDISPLAY_H
