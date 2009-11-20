#include "FFTGraph.h"
#include "TButton.h"
#include "TList.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TAxis.h"
#include <iostream>
using namespace std;

#include "FFTtools.h"


ClassImp(FFTGraph);

FFTGraph::FFTGraph()

  : TGraph(),fNewCanvas(0)
    
{
  this->SetEditable(kFALSE);
}


FFTGraph::FFTGraph(int N, const Int_t *x, const Int_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

FFTGraph::FFTGraph(int N, const Float_t *x, const Float_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{
  this->SetEditable(kFALSE);
}

FFTGraph::FFTGraph(int N, const Double_t *x, const Double_t *y)

  : TGraph(N,x,y),fNewCanvas(0)
{  
  this->SetEditable(kFALSE);
}

FFTGraph::~FFTGraph()
{
   //   std::cout << "~FFTGraph" << std::endl;
}

void FFTGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py)
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

void FFTGraph::drawInNewCanvas()
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
  FFTGraph *thisCopy = (FFTGraph*)this->Clone();
  thisCopy->GetXaxis()->SetLabelSize(0.06);
  thisCopy->GetXaxis()->SetTitleSize(0.06);
  thisCopy->GetYaxis()->SetLabelSize(0.06);
  thisCopy->GetYaxis()->SetTitleSize(0.06);
  thisCopy->GetXaxis()->SetTitle("Frequency (MHz)");
  thisCopy->GetYaxis()->SetTitle("dB (m maybe)");
  sprintf(graphTitle,"Channel %d",fChan+1);
  thisCopy->SetTitle(graphTitle);
  TCanvas *can = new TCanvas();
  can->SetLeftMargin(0.15);
  can->SetBottomMargin(0.15);
  can->SetTopMargin(0.1);
  can->SetRightMargin(0.1);
  thisCopy->Draw("al");
  //  fNewCanvas=1;
  
}

