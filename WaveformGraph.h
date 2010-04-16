#ifndef WAVEFORMGRAPH_H
#define WAVEFORMGRAPH_H
#include "TGraph.h"



class TObject;
class TGaxis;

class WaveformGraph : public TGraph
{

 public:
  WaveformGraph();
  WaveformGraph(TGraph *gr);
  WaveformGraph(int n, const Int_t *x, const Int_t *y);
  WaveformGraph(int n, const Float_t *x, const Float_t *y);
  WaveformGraph(int n, const Double_t *x, const Double_t *y);
  virtual ~WaveformGraph();
  
  void DrawFFT(); // *MENU*
  TGraph *getFFT();
  void setChannel(int chan) {fChan=chan;}
  void setPixelAndChannel(int pix, int chan) {fPixel=pix; fChan=chan;}

 private:
  Int_t fChan;
  Int_t fPixel;


  void ExecuteEvent(Int_t event, Int_t px, Int_t py);
  void ClearSelection(void);
  void ApplySelectionToButtons();
  void drawInNewCanvas();

  Int_t fNewCanvas;
  
  
  ClassDef(WaveformGraph,2)
};                              // end of class WaveformGraph


#endif  // WAVEFORMGRAPH_H
