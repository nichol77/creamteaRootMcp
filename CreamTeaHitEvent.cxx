#include "CreamTeaHitEvent.h"
#include "TGraph.h"


ClassImp(CreamTeaHitEvent);

TClonesArray *CreamTeaHitEvent::fgPmtHits=0;


CreamTeaHitEvent::CreamTeaHitEvent(){
  ///< Default Constructor
  
   // When the constructor is invoked for the first time, the class static
   // variable fgPmtHits is 0 and the TClonesArray fgPmtHits is created.

   if (!fgPmtHits) fgPmtHits = new TClonesArray("CreamTeaPmtHit", 1000);
   fPmtHits = fgPmtHits;
   fNumPmtHits=0;
}

CreamTeaHitEvent::CreamTeaHitEvent(MultiTargetModules *theMultiPtr, UInt_t timeSec, UInt_t timeUsec, UInt_t runNum, UInt_t eventNum)
{
 ///< Assignment Constructor

   // When the constructor is invoked for the first time, the class static
   // variable fgPmtHits is 0 and the TClonesArray fgPmtHits is created.
  if (!fgPmtHits) fgPmtHits = new TClonesArray("CreamTeaPmtHit", 1000);
  fPmtHits = fgPmtHits;

  fNumPmtHits=0;
  unixTime=timeSec;
  unixTimeUs=timeUsec;
  runNumber=runNum;
  eventNumber=eventNum;
  processEvent(theMultiPtr);
}

CreamTeaHitEvent::~CreamTeaHitEvent()
{
  ///< Destructor
  Clear();
}

void CreamTeaHitEvent::processEvent(MultiTargetModules *theMultiPtr)
{  
  for(int module=0;module<theMultiPtr->numTargetModules;module++) {
    for(int chip=0;chip<NUM_TARGETS;chip++) {
      for(int chan=0;chan<NUM_CHANNELS;chan++) {
	TGraph *grChan=theMultiPtr->getChannel(module,chip,chan);
	addPmtHitFromWaveform(grChan,module,chip,chan);


	//	pmtHit[getIndex(module,chip,chan)].fillHitInformation(grChan);
	delete grChan;
      }
    }
  }     
}

void CreamTeaHitEvent::addPmtHitFromWaveform(TGraph *grChan, Int_t module, Int_t chip, Int_t chan)
{
  TClonesArray &hits = *fPmtHits;
  CreamTeaPmtHit *pmthit = new(hits[fNumPmtHits++]) CreamTeaPmtHit(grChan,module,chip,chan);
  fLastHit = pmthit;
}


void CreamTeaHitEvent::Clear(Option_t * /*option*/)
{
   fPmtHits->Clear("C"); //Will also call CreamTeaPmtHit::Clear
}


//______________________________________________________________________________
void CreamTeaHitEvent::Reset(Option_t * /*option*/)
{
// Static function to reset all static objects for this event

   delete fgPmtHits; fgPmtHits = 0;
}
