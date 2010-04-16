#include "WaveformGraph.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#ifdef USE_FFT_TOOLS
#include "FFTtools.h"
#endif

ClassImp(WaveformGraph);

WaveformGraph::WaveformGraph(TGraph *gr) 
  : TGraph(*gr),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}


WaveformGraph::WaveformGraph()
  : TGraph(),fNewCanvas(0)
    
{
  this->SetEditable(kFALSE);
}


WaveformGraph::WaveformGraph(int N, const Int_t *x, const Int_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

WaveformGraph::WaveformGraph(int N, const Float_t *x, const Float_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

WaveformGraph::WaveformGraph(int N, const Double_t *x, const Double_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{  
  this->SetEditable(kFALSE);
}

WaveformGraph::~WaveformGraph()
{
   //   std::cout << "~WaveformGraph" << std::endl;
}

void WaveformGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   switch (event) {
   case kButtonPress:
     //     cout << "kButtonPress" << endl;
     break;
     
   case kButtonDoubleClick:
     //     std::cout << "kButtonDoubleClick" << std::endl;
     new TCanvas();
     break;

   case kButton1Down:
     //     std::cout << "kButton1Down" << std::endl;
     if(!fNewCanvas) drawInNewCanvas();
     else this->TGraph::ExecuteEvent(event,px,py);
     break;
          
   default:
       this->TGraph::ExecuteEvent(event,px,py);
       break;
   }
}

void WaveformGraph::drawInNewCanvas()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetOptTitle(1);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.1);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
  WaveformGraph *thisCopy = (WaveformGraph*)this->Clone();
  thisCopy->GetXaxis()->SetLabelSize(0.06);
  thisCopy->GetXaxis()->SetTitleSize(0.06);
  thisCopy->GetYaxis()->SetLabelSize(0.06);
  thisCopy->GetYaxis()->SetTitleSize(0.06);
  thisCopy->GetXaxis()->SetTitle("Time (ns)");
  thisCopy->GetYaxis()->SetTitle("Voltage (mV-ish)");
  sprintf(graphTitle,"Pixel %d, Channel %d",fPixel+1,fChan+1);
  thisCopy->SetTitle(graphTitle);
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  //  fNewCanvas=1;
  
}

//______________________________________________________________________________
void WaveformGraph::DrawFFT()
{
  char graphTitle[180];
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(1);
  gStyle->SetCanvasDefW(600);
  gStyle->SetCanvasDefH(400);
  //   gROOT->ForceStyle();
   
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);

  TGraph *grFFT  = this->getFFT();
  grFFT->GetXaxis()->SetLabelSize(0.06);
  grFFT->GetXaxis()->SetTitleSize(0.06);
  grFFT->GetYaxis()->SetLabelSize(0.06);
  grFFT->GetYaxis()->SetTitleSize(0.06);
  grFFT->GetXaxis()->SetTitle("Frequency (MHz)");
  grFFT->GetYaxis()->SetTitle("dB (m maybe)");
  sprintf(graphTitle,"Pixel %d, Channel %d",fPixel+1,fChan+1);
  //  sprintf(graphTitle,"Channel %d",fChan+1);
  grFFT->SetTitle(graphTitle);
  grFFT->Draw("al");
  
  //   printf("WaveformGraph::DrawFFT: not yet implemented\n");
}

TGraph *WaveformGraph::getFFT()
{
  #ifdef USE_FFT_TOOLS
  TGraph *grFFT = FFTtools::makePowerSpectrumMilliVoltsNanoSecondsdB(this);
  return grFFT;
  #endif
  return NULL;
}
