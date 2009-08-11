//////////////////////////////////////////////////////////////////////////////
/////  TargetDisplay.h        TargetDisplay                              /////
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
//#include "TargetDisplayConventions.h"

#include "Target.h" //The interface class

class TCanvas;
class TPad;
class TButton;
class TTreeIndex;
class TFile;
class TThread;

class TargetDisplay 
{
 public:
  
  TargetDisplay();
  ~TargetDisplay();
  
  static TargetDisplay* Instance();


  //Event playback mode
  void startEventPlaying();
  void stopEventPlaying();

  //Waveform event display stuff
  void startEventDisplay(); //these are 
  int displayNextEvent(); // all the
  void refreshEventDisplay(); // same
  void toggleView(Int_t view) {fView=view;}
  int getEventDisplayPlayMode() {return fInEventPlayMode;}
  void setEventDisplayPlayMode(int mode) { fInEventPlayMode=mode;}
  int getEventDisplayPlaySleep() {return fEventPlaySleepMs;}
  void drawEventButtons();

  Target *getTargetPointer() {return &(this->fTheTarget);}

 protected:
   static TargetDisplay *fgInstance;  
   // protect against multiple instances

 private:
   TCanvas *fTargetCanvas; //The main canvas
   TPad *fTargetMainPad;
   TPad *fTargetEventInfoPad;

   TButton *fWaveformButton;
   TButton *fPowerButton;

   Int_t fInEventPlayMode;
   Int_t fEventPlaySleepMs;
   Int_t fView; //1 is waveforms, 2 is FFTs

   Target fTheTarget;
};


#endif //TARGETDISPLAY_H
