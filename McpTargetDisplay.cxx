//////////////////////////////////////////////////////////////////////////////
/////  McpTargetDisplay.cxx                                                 /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for making pretty event canvases for TARGET_eval         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
//System includes
#include <fstream>
#include <iostream>

//McpTarget Display Includes
#include "McpTargetDisplay.h"
#include "WaveformGraph.h"
#ifdef USE_FFT_TOOLS
#include "FFTtools.h"
#endif
//ROOT Includes
#include "TROOT.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1.h"
#include "TButton.h"
#include "TGroupButton.h"
#include "TThread.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TSlider.h"
#include "TPaveText.h"
#include <TGClient.h>

using namespace std;


//Using 0-63 numbering
Int_t chanToPmtPixel[64]={7,8,6,9,5,10,4,11,3,12,2,
			  13,1,14,0,15,23,24,22,25,21,
			  26,20,27,19,28,18,29,17,30,16,
			  31,32,47,33,46,34,45,35,44,36,
			  43,37,42,38,41,39,40,48,63,49,
			  62,50,61,51,60,52,59,53,58,54,
			  57,55,56};

Int_t pmtPixelToChan[64]={14,12,10,8,6,4,2,0,1,3,5,7,9,11,13,15,30,28,26,24,22,20,18,16,17,19,21,23,25,27,29,31,32,34,36,38,40,42,44,46,47,45,43,41,39,37,35,33,48,50,52,54,56,58,60,62,63,61,59,57,55,53,51,49};


TGraph *getBoxCar(TGraph *grWave, Int_t halfWidth) 
{
  //Just do this the lazy way for now
  Double_t *inY = grWave->GetY();
  Double_t *inX = grWave->GetX();
  Int_t length=grWave->GetN();
  Double_t *smoothY = new Double_t[length];
  for(int i=0;i<length;i++) {
    smoothY[i]=0;
    if(i<halfWidth || length-i<=halfWidth) {
      int countVals=0;
      for(int j=i-halfWidth;j<=i+halfWidth;j++) {
	if(j>=0 && j<length) {
	  smoothY[i]+=inY[j];
	  countVals++;
	}
      }
      //      cout << i << "\t" << countVals << endl;
      smoothY[i]/=countVals;
    }
    else {
      for(int j=i-halfWidth;j<=i+halfWidth;j++) {
	smoothY[i]+=inY[j];
      }
      smoothY[i]/=1+2*halfWidth;
    }      
  }
  TGraph *grSmooth = new TGraph(length,inX,smoothY);
  delete [] smoothY;
  return grSmooth;
  
}



McpTargetDisplay*  McpTargetDisplay::fgInstance = 0;
//Leave these as global variables for now

McpTargetDisplay::McpTargetDisplay(int offlineMode,TFile *inputFile)
  :fTheMcpTarget(offlineMode),fOfflineMode(offlineMode)
{
  //Default constructor
  fInEventPlayMode=0;
  fEventPlaySleepMs=0;
  fgInstance=this;
  fMcpTargetCanvas=0;
  fMcpTargetEventInfoPad=0;
  fMcpTargetMainPad=0;
  fView=1;
  fAutoScaleMode=1;
  fAbsMaxScale=2096;
  fMinScale=-100;
  fMaxScale=100;
  fWhichModule=0;

  fTheOfflineFile=0;
  fTheOfflineTree=0;
  fTheMultiPtr=0;
  fTheMultiRawTargetModulesPtr=0;
  fTheOfflineEntry=0;
  if(inputFile) {
    setOfflineMode(inputFile);
  }
  fFarRightPave=0;
  fFarLeftPave=0;
  fMidLeftPave=0;
  fMidMidPave=0;
  fMidRightPave=0;

  
}

McpTargetDisplay::~McpTargetDisplay()
{
   std::cout << "McpTargetDisplay::~McpTargetDisplay()\n";
}





//______________________________________________________________________________
McpTargetDisplay*  McpTargetDisplay::Instance()
{
   //static function
   return (fgInstance) ? (McpTargetDisplay*) fgInstance : new McpTargetDisplay();
}


void McpTargetDisplay::setOfflineMode(TFile *inputFile)
{
  fOfflineMode=1;
  fTheOfflineFile = inputFile;
  if(!fTheOfflineFile) {
    std::cerr << "No input file -- giving up\n";
    exit(0);
  }
  fTheOfflineTree = (TTree*) fTheOfflineFile->Get("mcpTree");
  if(!fTheOfflineTree) {
    std::cerr << "No input tree -- giving up\n";
    exit(0);
  }
  fTheOfflineTree->SetBranchAddress("target",&fTheMultiRawTargetModulesPtr);
    

}

void McpTargetDisplay::startEventDisplay()
{ 
  //Read junk event
  if(!fOfflineMode) {
    fTheMcpTarget.readEvent();
    fTheMultiPtr=fTheMcpTarget.getTargetData(); //Do not delete
  }
  else {
  }
  this->displayNextEvent();   
}



void McpTargetDisplay::refreshEventDisplay()
{
   if(!fMcpTargetCanvas) {
      fMcpTargetCanvas = new TCanvas("canMcpTarget","canMcpTarget",1200,800);
      fMcpTargetCanvas->cd();
      fMcpTargetCanvas->SetBottomMargin(0);
      drawEventButtons();
   }
   if(!fMcpTargetMainPad) {
      fMcpTargetCanvas->cd();
      fMcpTargetMainPad= new TPad("canMcpTargetMain","canMcpTargetMain",0,0,1,0.9);
      fMcpTargetMainPad->Draw();
      fMcpTargetCanvas->Update();
   }
   if(!fMcpTargetEventInfoPad) {
      fMcpTargetCanvas->cd();
      fMcpTargetEventInfoPad= new TPad("canMcpTargetEventInfo","canMcpTargetEventInfo",0.15,0.91,0.85,0.99);
      fMcpTargetEventInfoPad->Draw();
      fMcpTargetCanvas->Update();
   } 
   //   std::cerr << "Here\n";

   //Update info pad with tings of interest
   char textLabel[180];
   fMcpTargetEventInfoPad->Clear();
   fMcpTargetEventInfoPad->Divide(5,1);
   fMcpTargetEventInfoPad->cd(1);
   if(fFarLeftPave) delete fFarLeftPave;
   fFarLeftPave = new TPaveText(0,0.01,1,1);
   fFarLeftPave->SetName("farLeftPave");
   fFarLeftPave->SetBorderSize(0);
   fFarLeftPave->SetFillColor(0);
   fFarLeftPave->SetTextAlign(13);
   sprintf(textLabel,"Module %d",fWhichModule);
   fFarLeftPave->AddText(textLabel);
   for(int chip=0;chip<NUM_TARGETS;chip++) {
     sprintf(textLabel,"Mem. Addr. %d -- %#x ",chip,fTheMultiPtr->targetData[fWhichModule].memAddrSpace[chip]);
     fFarLeftPave->AddText(textLabel);
   }

   fFarLeftPave->Draw();
   fMcpTargetEventInfoPad->cd(2);
   if(fMidLeftPave) delete fMidLeftPave;
   fMidLeftPave = new TPaveText(0,0.01,1,1);
   fMidLeftPave->SetName("midLeftPave");
   fMidLeftPave->SetBorderSize(0);
   fMidLeftPave->SetFillColor(0);
   fMidLeftPave->SetTextAlign(13);
   sprintf(textLabel,"Row -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].rowLoc[0],
	   fTheMultiPtr->targetData[fWhichModule].rowLoc[1],
	   fTheMultiPtr->targetData[fWhichModule].rowLoc[2],
	   fTheMultiPtr->targetData[fWhichModule].rowLoc[3]);
   fMidLeftPave->AddText(textLabel);
   sprintf(textLabel,"Col -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].colLoc[0],
	   fTheMultiPtr->targetData[fWhichModule].colLoc[1],
	   fTheMultiPtr->targetData[fWhichModule].colLoc[2],
	   fTheMultiPtr->targetData[fWhichModule].colLoc[3]);
   fMidLeftPave->AddText(textLabel);
   sprintf(textLabel,"HITBIT -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].hitBit[0],
	   fTheMultiPtr->targetData[fWhichModule].hitBit[1],
	   fTheMultiPtr->targetData[fWhichModule].hitBit[2],
	   fTheMultiPtr->targetData[fWhichModule].hitBit[3]);
   fMidLeftPave->AddText(textLabel);
   sprintf(textLabel,"Thresh -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].thresh[0],
	   fTheMultiPtr->targetData[fWhichModule].thresh[1],
	   fTheMultiPtr->targetData[fWhichModule].thresh[2],
	   fTheMultiPtr->targetData[fWhichModule].thresh[3]);
   fMidLeftPave->AddText(textLabel);
   fMidLeftPave->Draw();
   fMcpTargetEventInfoPad->cd(3);
   if(fMidMidPave) delete fMidMidPave;
   fMidMidPave = new TPaveText(0,0.01,1,1);
   fMidMidPave->SetName("midMidPave");
   fMidMidPave->SetBorderSize(0);
   fMidMidPave->SetFillColor(0);
   fMidMidPave->SetTextAlign(13);
  //  sprintf(textLabel,"ROVDD -- %2.2f,%2.2f,%2.2f,%2.2f",
// 	   fTheMultiPtr->targetData[fWhichModule].rovdd[0],
// 	   fTheMultiPtr->targetData[fWhichModule].rovdd[1],
// 	   fTheMultiPtr->targetData[fWhichModule].rovdd[2],
// 	   fTheMultiPtr->targetData[fWhichModule].rovdd[3]);
   sprintf(textLabel,"ROVDD -- %#x %#x %#x %#x",
	   fTheMultiPtr->targetData[fWhichModule].raw_rovdd[0],
	   fTheMultiPtr->targetData[fWhichModule].raw_rovdd[1],
	   fTheMultiPtr->targetData[fWhichModule].raw_rovdd[2],
	   fTheMultiPtr->targetData[fWhichModule].raw_rovdd[3]);
   fMidMidPave->AddText(textLabel);
   sprintf(textLabel,"Scaler0 -- %u,%u,%u,%u",
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[0][0],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[1][0],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[2][0],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[3][0]);
   fMidMidPave->AddText(textLabel);
   sprintf(textLabel,"Scaler1 -- %u,%u,%u,%u",
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[0][0],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[1][1],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[2][1],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[3][1]);
   fMidMidPave->AddText(textLabel);
   sprintf(textLabel,"Scaler2 -- %u,%u,%u,%u",
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[0][2],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[1][2],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[2][2],
	   fTheMultiPtr->targetData[fWhichModule].raw_scaler[3][2]);
   fMidMidPave->AddText(textLabel);
//    sprintf(textLabel,"Scaler0 -- %3.2f,%3.2f,%3.2f,%3.2f",
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[0][0],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[1][0],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[2][0],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[3][0]);
//    fMidMidPave->AddText(textLabel);
//    sprintf(textLabel,"Scaler1 -- %3.2f,%3.2f,%3.2f,%3.2f",
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[0][0],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[1][1],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[2][1],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[3][1]);
//    fMidMidPave->AddText(textLabel);
//    sprintf(textLabel,"Scaler2 -- %3.1f,%3.1f,%3.1f,%3.1f",
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[0][2],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[1][2],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[2][2],
// 	   fTheMultiPtr->targetData[fWhichModule].scaler[3][2]);
//    fMidMidPave->AddText(textLabel);
   fMidMidPave->Draw();
   fMcpTargetEventInfoPad->cd(4);
   if(fMidRightPave) delete fMidRightPave;
   fMidRightPave = new TPaveText(0,0.01,1,1);
   fMidRightPave->SetName("midRightPave");
   fMidRightPave->SetBorderSize(0);
   fMidRightPave->SetFillColor(0);
   fMidRightPave->SetTextAlign(13);
   sprintf(textLabel,"FB -- %#x,%#x,%#x,%#x",
	   fTheMultiPtr->targetData[fWhichModule].feedback[0],
	   fTheMultiPtr->targetData[fWhichModule].feedback[1],
	   fTheMultiPtr->targetData[fWhichModule].feedback[2],
	   fTheMultiPtr->targetData[fWhichModule].feedback[3]);
   fMidRightPave->AddText(textLabel);
   sprintf(textLabel,"WB -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].wbias[0],
	   fTheMultiPtr->targetData[fWhichModule].wbias[1],
	   fTheMultiPtr->targetData[fWhichModule].wbias[2],
	   fTheMultiPtr->targetData[fWhichModule].wbias[3]);
   fMidRightPave->AddText(textLabel);
   sprintf(textLabel,"EnPed -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].enPed[0],
	   fTheMultiPtr->targetData[fWhichModule].enPed[1],
	   fTheMultiPtr->targetData[fWhichModule].enPed[2],
	   fTheMultiPtr->targetData[fWhichModule].enPed[3]);
   fMidRightPave->AddText(textLabel);
   sprintf(textLabel,"Term -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].term[0],
	   fTheMultiPtr->targetData[fWhichModule].term[1],
	   fTheMultiPtr->targetData[fWhichModule].term[2],
	   fTheMultiPtr->targetData[fWhichModule].term[3]);
   fMidRightPave->AddText(textLabel);
   fMidRightPave->Draw();
   fMcpTargetEventInfoPad->cd(5);
   if(fFarRightPave) delete fFarRightPave;
   fFarRightPave = new TPaveText(0,0.01,1,1);
   fFarRightPave->SetName("farRightPave");
   fFarRightPave->SetBorderSize(0);
   fFarRightPave->SetFillColor(0);
   fFarRightPave->SetTextAlign(13);
   sprintf(textLabel,"Temp - %3.1f,%3.1f,%3.1f,%3.1fC",
	   fTheMultiPtr->targetData[fWhichModule].temperature[0],
	   fTheMultiPtr->targetData[fWhichModule].temperature[1],
	   fTheMultiPtr->targetData[fWhichModule].temperature[2],
	   fTheMultiPtr->targetData[fWhichModule].temperature[3]);
   fFarRightPave->AddText(textLabel);
   sprintf(textLabel,"Sign -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].sign[0],
	   fTheMultiPtr->targetData[fWhichModule].sign[1],
	   fTheMultiPtr->targetData[fWhichModule].sign[2],
	   fTheMultiPtr->targetData[fWhichModule].sign[3]);
   fFarRightPave->AddText(textLabel);
   sprintf(textLabel,"PedRowAddr -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].pedRowAddr[0],
	   fTheMultiPtr->targetData[fWhichModule].pedRowAddr[1],
	   fTheMultiPtr->targetData[fWhichModule].pedRowAddr[2],
	   fTheMultiPtr->targetData[fWhichModule].pedRowAddr[3]);
   fFarRightPave->AddText(textLabel);
   sprintf(textLabel,"PedColAddr -- %d,%d,%d,%d",
	   fTheMultiPtr->targetData[fWhichModule].pedColAddr[0],
	   fTheMultiPtr->targetData[fWhichModule].pedColAddr[1],
	   fTheMultiPtr->targetData[fWhichModule].pedColAddr[2],
	   fTheMultiPtr->targetData[fWhichModule].pedColAddr[3]);
   fFarRightPave->AddText(textLabel);
   fFarRightPave->Draw();
   
   
   //   std::cerr << "Here\n";


   static TGraph *gr[NUM_TOTAL_CHANNELS]={0};
   static WaveformGraph *wv[NUM_TOTAL_CHANNELS]={0};

#ifdef USE_FFT_TOOLS
   static TGraph *fft[NUM_TOTAL_CHANNELS]={0};
#endif
   //For now lets be lazy
   static int padsDrawn=0;
   static TPad *padGraphs=0;
   if(fView==2) 
     padsDrawn=0;
   if(!padsDrawn) {
     fMcpTargetMainPad->Clear();
     fMcpTargetMainPad->cd();
     fMcpTargetMainPad->SetBottomMargin(0);
     drawZoomButtons();
     padGraphs= new TPad("padGraphs","padGraphs",0.05,0.,0.95,0.95);
     padGraphs->Draw();
     padGraphs->Divide(8,8,0,0);
     
     //Now add labels
     TLatex texy;
     texy.SetTextSize(0.03); 
     texy.SetTextAlign(12);  
     for(int column=0;column<8;column++) {
       sprintf(textLabel,"%d",1+column);
       texy.DrawTextNDC((column+1)*0.115,0.97,textLabel);
     }
     for(int row=0;row<8;row++) {
       sprintf(textLabel,"%d",1+(8*row));
       texy.DrawTextNDC(0.02,1-((row+1)*0.114),textLabel);
     }
   }

   //   std::cerr << "Here fred\n";
   
   //   fMcpTargetMainPad->Divide(8,8,0,0);
   Double_t maxVal=0;
   char graphName[180];
   Double_t minTime=0;
   Double_t maxTime=NUM_SAMPLES;
   //   Double_t minFreq=0;
   //   Double_t maxFreq=500;
   for(int pixel=0;pixel<NUM_TOTAL_CHANNELS;pixel++) {
      //      RJN change 28/04010
     Int_t chan=pixel;
     //      Int_t chan=pmtPixelToChan[pixel];
     //     std::cerr << pixel << "\t" << chan << "\n";
     sprintf(graphName,"Pixel %d",pixel+1);
     if(gr[pixel]) delete gr[pixel];
     if(wv[pixel]) delete wv[pixel];

#ifdef USE_FFT_TOOLS
     if(fft[pixel]) delete fft[pixel];
#endif

     //     std::cerr << fTheMultiPtr << "\n";
     TGraph *grTemp =fTheMultiPtr->getChannel(fWhichModule,chan);
     gr[pixel] = getBoxCar(grTemp,3);
     delete grTemp;
     //     std::cerr << pixel << "\t" << gr[pixel] << "\n";
     wv[pixel] = new WaveformGraph(gr[pixel]);
     //     std::cerr << "Waveform: " << pixel << "\t" << wv[pixel] << "\n";
     wv[pixel]->setPixelAndChannel(pixel,chan);
     wv[pixel]->SetLineColor(9);

#ifdef USE_FFT_TOOLS
     fft[pixel] = (wv[pixel]->getFFT());
     fft[pixel]->SetLineColor(9);
#endif
     //     std::cerr << gr[pixel] << "\t" << wv[pixel] << "\t"  << "\n";

     //     Double_t sqVal=FFTtools::getPeakSqVal(wv[pixel]);
     Double_t *yVals=wv[pixel]->GetY();
     Double_t sqVal=0;
     for(int i=0;i<wv[pixel]->GetN();i++) {
       Double_t tempVal=yVals[i]*yVals[i];
       if(sqVal<tempVal) sqVal=tempVal;
     }
     //     std::cout << pixel << "\t" << sqVal << "\n"; 
     if(sqVal>maxVal)
       maxVal=sqVal;
     wv[pixel]->SetTitle(graphName);
#ifdef USE_FFT_TOOLS
     fft[pixel]->SetTitle(graphName);
#endif

   }
   gStyle->SetTitle(0);
   gStyle->SetLabelSize(0.1,"xy");
   gStyle->SetTitleSize(0.1,"x");
   maxVal=TMath::Sqrt(maxVal);
   if(maxVal>4000) maxVal=4000;
   for(int pixel=0;pixel<NUM_TOTAL_CHANNELS;pixel++) {
     padGraphs->cd(pixel+1);
     if(!padsDrawn) {
       if((pixel+1)%8==0)
	 gPad->SetRightMargin(0.01);
       if(pixel>=56) {
	 gPad->SetBottomMargin(0.2);       
       }
     }
     wv[pixel]->SetMaximum(maxVal*1.2);
     wv[pixel]->SetMinimum(-1.2*maxVal);
     
     if(fView==1)  {
       if(!padsDrawn) {
	 TH1F *framey =0;
	 if(fAutoScaleMode) 
	   framey = gPad->DrawFrame(minTime,-1.2*maxVal,maxTime,1.2*maxVal);
	 else 
	   framey = gPad->DrawFrame(minTime,fMinScale,maxTime,fMaxScale);
	 if(pixel>=56) {
	   framey->GetYaxis()->SetLabelSize(0.08);
	 }
	 framey->SetXTitle("Time (ns)");       
       }
       else {
	 TList *listy = gPad->GetListOfPrimitives();
	 for(int i=0;i<listy->GetSize();i++) {
	   TObject *fred = listy->At(i);
	   TH1F *tempHist = (TH1F*) fred;
	   if(tempHist->InheritsFrom("TH1")) {
	     if(fAutoScaleMode)
	       tempHist->GetYaxis()->SetRangeUser(-1.2*maxVal,1.2*maxVal);
	     else
	       tempHist->GetYaxis()->SetRangeUser(fMinScale,fMaxScale);
	   }
	 }
	   
       }
       wv[pixel]->Draw("l");
     }
#ifdef USE_FFT_TOOLS
     if(fView==2)
       fft[pixel]->Draw("al");
#endif

   }

   padsDrawn=1;
  
  fMcpTargetCanvas->Update();
}

int McpTargetDisplay::displayNextEvent()
{  
  Int_t gotEvent=0;
  if(!fOfflineMode) {
    gotEvent=fTheMcpTarget.readEvent();
    fTheMultiPtr=fTheMcpTarget.getTargetData(); //Do not delete
  }
  else {
    //    std::cout << fTheOfflineEntry << "\t" << fTheOfflineTree->GetEntries() << "\n";
    if(fTheOfflineEntry<fTheOfflineTree->GetEntries()) {
      fTheOfflineTree->GetEntry(fTheOfflineEntry);
      if(fTheMultiPtr) 
	delete fTheMultiPtr;
      //      std::cerr << fTheMultiPtr << "\t" <<fTheMultiRawTargetModulesPtr << "\n";
      //      std::cerr << fTheOfflineEntry << "\t" << fTheMultiRawTargetModulesPtr->raw[3] << "\n";
      fTheMultiPtr=new MultiTargetModules(fTheMultiRawTargetModulesPtr);
      fTheMcpTarget.fillVoltageArray(fTheMultiPtr);
      //      std::cerr << fTheOfflineEntry << "\t" << fTheMultiPtr->raw[3] << "\t" << fTheMultiPtr->data[0][0][0] << "\t" << fTheMultiPtr->fVoltBuffer[0][0][0] <<  "\n";

      fTheOfflineEntry++;
      gotEvent=1;
    }    
  }
  if(gotEvent==1)
    refreshEventDisplay(); 
  else {
    //Didn't got event
  }
  return gotEvent;
}


int McpTargetDisplay::displayPreviousEvent()
{  
  Int_t gotEvent=0;
  if(!fOfflineMode) {
    //    gotEvent=fTheMcpTarget.readEvent();
    //    fTheMultiPtr=fTheMcpTarget.getTargetData(); //Do not delete
    return gotEvent;
  }
  else {
    //    std::cout << fTheOfflineEntry << "\t" << fTheOfflineTree->GetEntries() << "\n";
    if(fTheOfflineEntry>1) {
      fTheOfflineEntry-=2;      
      fTheOfflineTree->GetEntry(fTheOfflineEntry);
      if(fTheMultiPtr) 
	delete fTheMultiPtr;
      //      std::cerr << fTheMultiPtr << "\t" <<fTheMultiRawTargetModulesPtr << "\n";
      //      std::cerr << fTheOfflineEntry << "\t" << fTheMultiRawTargetModulesPtr->raw[3] << "\n";
      fTheMultiPtr=new MultiTargetModules(fTheMultiRawTargetModulesPtr);
      fTheMcpTarget.fillVoltageArray(fTheMultiPtr);
      //      std::cerr << fTheOfflineEntry << "\t" << fTheMultiPtr->raw[3] << "\t" << fTheMultiPtr->data[0][0][0] << "\t" << fTheMultiPtr->fVoltBuffer[0][0][0] <<  "\n";

      fTheOfflineEntry++;
      gotEvent=1;
    }    
  }
  if(gotEvent==1)
    refreshEventDisplay(); 
  else {
    //Didn't got event
  }
  return gotEvent;
}




void McpTargetDisplay::drawEventButtons() {
  if(!fOfflineMode) {
   TButton *butSave = new TButton("Save ","McpTargetDisplay::Instance()->saveTree();",0,0.97,0.05,1);
   butSave->SetTextSize(0.5);
   butSave->SetFillColor(kBlue-3);
   butSave->Draw();
  }


   if(fOfflineMode) {      
      TButton *butNext = new TButton("> ","McpTargetDisplay::Instance()->displayNextEvent();",0.975,0.97,1,1);
      butNext->SetTextSize(0.5);
      butNext->SetFillColor(kGreen-10);
      butNext->Draw();
      TButton *butPrev = new TButton("<","McpTargetDisplay::Instance()->displayPreviousEvent();",0.95,0.97,0.975,1);
      butPrev->SetTextSize(0.5);
      butPrev->SetFillColor(kRed-10);
      butPrev->Draw();
   }
   else {

      TButton *butNext = new TButton("Next ","McpTargetDisplay::Instance()->displayNextEvent();",0.95,0.97,1,1);
      butNext->SetTextSize(0.5);
      butNext->SetFillColor(kGreen-10);
      butNext->Draw();
   }

   

   TButton *butPlay = new TButton("Play","McpTargetDisplay::Instance()->startEventPlaying();",0.95,0.93,1,0.96);
   butPlay->SetTextSize(0.5);
   butPlay->SetFillColor(kGreen-10);
   butPlay->Draw();
   TButton *butStop = new TButton("Stop","McpTargetDisplay::Instance()->stopEventPlaying();",0.95,0.9,1,0.93);
   butStop->SetTextSize(0.5);
   butStop->SetFillColor(kRed-10);
   butStop->Draw();

   
   fModuleButton = new TButton("Module","McpTargetDisplay::Instance()->toggleModule();",0.9,0.95,0.95,1);
   fModuleButton->SetTextSize(0.4);
   fModuleButton->SetFillColor(kBlue-8);
   fModuleButton->Draw();


   //NEW BUTTONS
   fWaveformButton = new TButton("Waveform View","McpTargetDisplay::Instance()->toggleView(1); McpTargetDisplay::Instance()->refreshEventDisplay();",0.05,0.95,0.14,1);
   fWaveformButton->SetTextSize(0.4);
   fWaveformButton->SetFillColor(kGray+3);
   fWaveformButton->Draw();
#ifdef USE_FFT_TOOLS
   fPowerButton = new TButton("FFT View","McpTargetDisplay::Instance()->toggleView(2); McpTargetDisplay::Instance()->refreshEventDisplay();",0.05,0.9,0.14,0.95);
   fPowerButton->SetTextSize(0.4);
   fPowerButton->SetFillColor(kGray);
   fPowerButton->Draw();
#endif

 //   if(!fOfflineMode) {
//      fThresholdSlider = new TSlider("threshSlider","thresh",0.9,0.9,0.95,1);
//      fThresholdSlider->SetMethod("McpTargetDisplay::Instance()->updateThresholdFromSlider();");
//      fThresholdSlider->SetRange(0,0.2);
//      fThresholdSlider->SetEditable(0);
//      updateThreshold(fTheMcpTarget.getTrigThresh());
//    }

}



void McpTargetDisplay::drawZoomButtons() {


   //NEW BUTTONS
   fZoomButton = new TButton("Autoscale","McpTargetDisplay::Instance()->toggleAutoscale(); McpTargetDisplay::Instance()->refreshEventDisplay();",0.95,0.9,1,0.95);
   fZoomButton->SetTextSize(0.3);
   fZoomButton->SetFillColor(kGray+3);
   fZoomButton->Draw();

   fZoomSlider = new TSlider("zoomSlider","zoom",0.95,0.05,1,0.85);
   fZoomSlider->SetMethod("McpTargetDisplay::Instance()->setFixedRange(); McpTargetDisplay::Instance()->refreshEventDisplay();");
   fZoomSlider->SetRange(0,1);
   fZoomSlider->SetEditable(0);

}



void McpTargetDisplay::startEventPlaying()
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




void McpTargetDisplay::stopEventPlaying() 
{
  fInEventPlayMode=0;
}

void McpTargetDisplay::updateThresholdFromSlider() 
{
  //  std::cout << fThresholdSlider->GetMinimum() << "\t" 
  //	    << fThresholdSlider->GetMaximum() << "\n";

  //0.2, 1  --> Spans range 0,4095
 //  Double_t minVal=fThresholdSlider->GetMinimum();
//   Double_t maxVal=fThresholdSlider->GetMaximum();
//   Double_t range=maxVal-minVal;
//   Double_t dThresh=((maxVal-range)/(1.0-range))*4095;
//   UInt_t thresh=(UInt_t)dThresh;
//   updateThreshold(thresh);

}

void McpTargetDisplay::updateThreshold(UInt_t threshold) 
{
  //Threshold thingies
  char textLabel[180];
  sprintf(textLabel,"Threshold: %u",threshold);
 //  fThresholdSlider->SetToolTipText(textLabel,0);
//   fTheMcpTarget.setTrigThresh(threshold);
}

void McpTargetDisplay::toggleAutoscale() 
{ 
  fAutoScaleMode=1-fAutoScaleMode;
  if(fAutoScaleMode) {
    fZoomButton->SetFillColor(kGray+3);
    fZoomButton->Modified();
  }
  else {
    fZoomButton->SetFillColor(kGray);
    fZoomButton->Modified();
  }
    
}

void McpTargetDisplay::setFixedRange() 
{
  //  std::cout << fThresholdSlider->GetMinimum() << "\t" 
  //	    << fThresholdSlider->GetMaximum() << "\n";

  //0.2, 1  --> Spans range 0,4095
  Double_t minVal=fZoomSlider->GetMinimum();
  Double_t maxVal=fZoomSlider->GetMaximum();
  
  
  fMinScale=(minVal-0.5)*fAbsMaxScale;
  fMaxScale=(maxVal-0.5)*fAbsMaxScale;
  

}
void McpTargetDisplay::setFixedRange(Double_t minScale,Double_t maxScale) 
{
 
  
   fAutoScaleMode=0;
   fMinScale=minScale;
   fMaxScale=maxScale;
   refreshEventDisplay();
  
}

void McpTargetDisplay::toggleModule() 
{
   int maxModule=MAX_TARGET_MODULES;
   if(fTheMultiPtr) maxModule=fTheMultiPtr->getNumModules();
   fWhichModule++;
   if(fWhichModule>=maxModule) fWhichModule=0;
   refreshEventDisplay();
}
