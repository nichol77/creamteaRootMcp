//////////////////////////////////////////////////////////////////////////////
/////  CreamTeaHitEvent        CREAM TEA Hit Event Class                 /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing information about CREAM TEA PMT     /////
/////     hits in  a TTree                                               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////



#ifndef CREAMTEAHITEVENT_H
#define CREAMTEAHITEVENT_H



#include "Defs.h"
#include "McpTarget.h"
#include "MultiRawTargetModules.h"
#include "MultiTargetModules.h"
#include "CreamTeaPmtHit.h"

#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TMultiGraph.h"
#include "TClonesArray.h"
#include "TRef.h"




class CreamTeaHitEvent : public TObject 
{
 public:
  CreamTeaHitEvent(); ///< Default Constructor
  CreamTeaHitEvent(MultiTargetModules *theMultiPtr, UInt_t timeSec, UInt_t timeUsec, UInt_t runNumber, UInt_t eventNumber); ///< Assignment Constructor
  ~CreamTeaHitEvent(); ///< Destructor
  void processEvent(MultiTargetModules *theMultiPtr); ///<Loops over channels and fills the hit information
  void addPmtHitFromWaveform(TGraph *grChan, Int_t module, Int_t chip, Int_t chan);
  void Clear(Option_t *option="");
  static void   Reset(Option_t *option ="");
   


  ///< Event variables
  UInt_t unixTime; ///< Event readout time in seconds since the epoch
  UInt_t unixTimeUs; ///< Event readout time in microseconds within the second
  UInt_t runNumber; ///<  Run number
  UInt_t eventNumber; ///< Event number

  //  CreamTeaPmtHit pmtHit[MAX_TARGET_MODULES*NUM_TARGETS*NUM_CHANNELS];  ///< Array of PMT hit information

  Int_t fNumPmtHits;
  TClonesArray *fPmtHits; ///< Array with (currently) all the PMT hits

  static TClonesArray *fgPmtHits;

   TRef           fLastHit;         ///<reference pointer to last PMT hit


  ClassDef(CreamTeaHitEvent,1); 
};

#endif //CREAMTEAHITEVENT_H
