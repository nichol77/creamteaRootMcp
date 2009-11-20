//////////////////////////////////////////////////////////////////////////////
/////  TargetDisplay.cxx                                                 /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for TARGET_eval         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
//System includes
#include <fstream>
#include <iostream>

//Target Display Includes
#include "TargetDisplay.h"
#include "WaveformGraph.h"
#include "FFTtools.h"

//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include <TGClient.h>

using namespace std;

TargetDisplay*  TargetDisplay::fgInstance = 0;
//Leave these as global variables for now

TargetDisplay::TargetDisplay()
{
  //Default constructor
  fInEventPlayMode=0;
  fEventPlaySleepMs=0;
  fgInstance=this;
  fTargetCanvas=0;
  fTargetEventInfoPad=0;
  fTargetMainPad=0;
  fView=1;
  
}

TargetDisplay::~TargetDisplay()
{
   //Default destructor
}





//______________________________________________________________________________
TargetDisplay*  TargetDisplay::Instance()
{
   //static function
   return (fgInstance) ? (TargetDisplay*) fgInstance : new TargetDisplay();
}

void TargetDisplay::startEventDisplay()
{ 
  //Read junk event
  fTheTarget.readEvent();
  this->displayNextEvent();   
}



void TargetDisplay::refreshEventDisplay()
{
   if(!fTargetCanvas) {
      fTargetCanvas = new TCanvas("canTarget","canTarget",1200,800);
      fTargetCanvas->cd();
      drawEventButtons();
   }
   if(!fTargetMainPad) {
      fTargetCanvas->cd();
      fTargetMainPad= new TPad("canTargetMain","canTargetMain",0,0,1,0.9);
      fTargetMainPad->Draw();
      fTargetCanvas->Update();
   }
   if(!fTargetEventInfoPad) {
      fTargetCanvas->cd();
      fTargetEventInfoPad= new TPad("canTargetEventInfo","canTargetEventInfo",0.2,0.91,0.8,0.99);
      fTargetEventInfoPad->Draw();
      fTargetCanvas->Update();
   } 

   static TGraph *gr[NUM_CHANNELS]={0};
   static WaveformGraph *wv[NUM_CHANNELS]={0};
   static TGraph *fft[NUM_CHANNELS]={0};

   //For now lets be lazy
   fTargetMainPad->Clear();
   fTargetMainPad->Divide(4,4);
   Double_t maxVal=0;
   char graphName[180];
   for(int chan=0;chan<16;chan++) {
     sprintf(graphName,"Channel %d",chan+1);
     if(gr[chan]) delete gr[chan];
     if(wv[chan]) delete wv[chan];
     if(fft[chan]) delete fft[chan];
     gr[chan] = fTheTarget.getChannel(chan);
     wv[chan] = new WaveformGraph(gr[chan]);
     wv[chan]->setChannel(chan);
     wv[chan]->SetLineColor(9);
     fft[chan] = (wv[chan]->getFFT());
     fft[chan]->SetLineColor(9);

     if(FFTtools::getPeakSqVal(wv[chan])>maxVal)
       maxVal=FFTtools::getPeakSqVal(wv[chan]);
     wv[chan]->SetTitle(graphName);
     fft[chan]->SetTitle(graphName);
     
   }
   maxVal=TMath::Sqrt(maxVal);
   for(int chan=0;chan<16;chan++) {
     fTargetMainPad->cd(chan+1);
     wv[chan]->SetMaximum(maxVal*1.2);
     wv[chan]->SetMinimum(-1.2*maxVal);
     if(fView==1) 
       wv[chan]->Draw("al");
     if(fView==2)
       fft[chan]->Draw("al");
   }

  
  fTargetCanvas->Update();
}

int TargetDisplay::displayNextEvent()
{  
  Int_t gotEvent=fTheTarget.readEvent();
  if(gotEvent==1)
    refreshEventDisplay(); 
  else {
    //Didn't got event
  }
  return gotEvent;
}




void TargetDisplay::drawEventButtons() {
   TButton *butNext = new TButton("Next ","TargetDisplay::Instance()->displayNextEvent();",0.95,0.975,1,1);
   butNext->SetTextSize(0.5);
   butNext->SetFillColor(kGreen-10);
   butNext->Draw();
   

   TButton *butPlay = new TButton("Play","TargetDisplay::Instance()->startEventPlaying();",0.9,0.95,0.95,1);
   butPlay->SetTextSize(0.5);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butStop = new TButton("Stop","TargetDisplay::Instance()->stopEventPlaying();",0.90,0.90,0.95,0.94);
   butStop->SetTextSize(0.5);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();


   //NEW BUTTONS
   fWaveformButton = new TButton("Waveform View","TargetDisplay::Instance()->toggleView(1); TargetDisplay::Instance()->refreshEventDisplay();",0.05,0.95,0.14,1);
   fWaveformButton->SetTextSize(0.4);
   fWaveformButton->SetFillColor(kGray+3);
   fWaveformButton->Draw();
   fPowerButton = new TButton("FFT View","TargetDisplay::Instance()->toggleView(2); TargetDisplay::Instance()->refreshEventDisplay();",0.05,0.9,0.14,0.95);
   fPowerButton->SetTextSize(0.4);
   fPowerButton->SetFillColor(kGray);
   fPowerButton->Draw();

}



void TargetDisplay::startEventPlaying()
{

  fInEventPlayMode=1;
  do {
    gSystem->ProcessEvents();
    if(!fInEventPlayMode) break;
    if(fEventPlaySleepMs>0)
      gSystem->Sleep(fEventPlaySleepMs);
  }
  while(this->displayNextEvent()==1);
}




void TargetDisplay::stopEventPlaying() 
{
  fInEventPlayMode=0;
}

