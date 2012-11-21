//////////////////////////////////////////////////////////////////////////////
/////  CreamTeaPmtHit        CREAM TEA Pmt Hit Class                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for extracting and storing information about    /////
/////     CREAM TEA PMT hits.                                            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////



#ifndef CREAMTEAPMTHIT_H
#define CREAMTEAPMTHIT_H



#include "Defs.h"
#include "McpTarget.h"
#include "MultiRawTargetModules.h"
#include "MultiTargetModules.h"
#include "TGraph.h"



class CreamTeaPmtHit : public TObject
{
 public:
  CreamTeaPmtHit(); ///< Default constructor
  CreamTeaPmtHit(TGraph *grChan, Int_t module, Int_t chip, Int_t chan); ///< Assignment consturctor
  ~CreamTeaPmtHit(); ///< Destructor

  static Int_t getIndex(Int_t module, Int_t chip, Int_t chan)   
    { return chan+NUM_CHANNELS*chip+NUM_CHANNELS*NUM_TARGETS*module;} 
  

  Int_t chanIndex; ///< Which channel is it?
  Float_t pulseArea;  ///< The area of the pulse
  Float_t pulseVal; ///< The peak of the pulse
  Int_t peakSamp; ///< The sample number of the peak
  Int_t numPulseSamps; ///< The number of samples in the pulse;
 
  void fillHitInformation(TGraph *gr);
  TGraph *getBoxCar(TGraph *grWave, Int_t halfWidth) ;
  ClassDef(CreamTeaPmtHit,1);
};

#endif // CREAMTEAPMTHIT_H
