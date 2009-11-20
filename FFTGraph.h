#ifndef FFTGRAPH_H
#define FFTGRAPH_H
#include "TGraph.h"



class TObject;
class TGaxis;

class FFTGraph : public TGraph
{

 public:
  FFTGraph();  
  FFTGraph(int n, const Int_t *x, const Int_t *y);
  FFTGraph(int n, const Float_t *x, const Float_t *y);
  FFTGraph(int n, const Double_t *x, const Double_t *y);
  virtual ~FFTGraph();
  

  void setChannel(int chan) {fChan=chan;}

 private:
  Int_t fChan;


  void ExecuteEvent(Int_t event, Int_t px, Int_t py);
  void ClearSelection(void);
  void ApplySelectionToButtons();
  void drawInNewCanvas();

  Int_t fNewCanvas;
  
  
  ClassDef(FFTGraph,2)
};                              // end of class FFTGraph


#endif  // FFTGRAPH_H
