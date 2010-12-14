#include <fstream>
#include <iostream>
#include "TTree.h"
#include "TFile.h"


#define MAX_TARGET_MODULES    4        // Number of 64 channel target modules
#define NUM_TARGETS           4        // Number of target chips
#define NUM_CHANNELS          16       // Number of channels
#define NUM_TOTAL_CHANNELS    NUM_TARGETS*NUM_CHANNELS //64   
#define NUM_PIXELS            64 //64   
#define NUM_ROWS              8        // Rows per channel
#define NUM_COLS              32       // Columns per row
#define SAMPLES_PER_COL        512   //Amount of values pe



void plotPedestal()
{

   Float_t fPedestalValues[MAX_TARGET_MODULES][NUM_TARGETS][NUM_CHANNELS][NUM_ROWS][NUM_COLS][SAMPLES_PER_COL];
   Float_t value;
   std::ifstream PedFile;
   PedFile.open("pedestal.txt");
   if(!PedFile.is_open()) {
      PedFile.open("defaultPed.txt");
   }
   else {
      std::cout << "Read pedestal.txt\n";
  }
   
   
   int module,chip,chan,row,col,samp;
   int capId;
   TFile *fp = new TFile("simpPed.root","RECREATE");
   TTree *simpPed = new TTree("simpPed","simpPed");
   simpPed->Branch("module",&module,"module/I");
   simpPed->Branch("chip",&chip,"chip/I");
   simpPed->Branch("chan",&chan,"chan/I");
   simpPed->Branch("row",&row,"row/I");
   simpPed->Branch("col",&col,"col/I");
   simpPed->Branch("samp",&samp,"samp/I");
   simpPed->Branch("capId",&capId,"capId/I");
   simpPed->Branch("value",&value,"value/F");

   if(PedFile.is_open()) {
      for(module=0;module<MAX_TARGET_MODULES;module++) {
	 for(chip=0;chip<NUM_TARGETS;chip++) {
	    for(chan=0;chan<NUM_CHANNELS;chan++) {
	       for(row=0;row<NUM_ROWS;row++) {
		  for(col=0;col<NUM_COLS;col+=2) {
		     for(samp=0;samp<SAMPLES_PER_COL;samp++) {
			capId=(512*row)+(16*col)+samp;
			capId=capId%4096;
			PedFile >> value;
			fPedestalValues[module][chip][chan][row][col][samp]=value;
			simpPed->Fill();
			if(module<2) {
			   if(value<600 || value>1600) 
			   std:cout << module << "\t" << chip << "\t" << chan
				    << "\t" << row << "\t" << col << "\t"
				    << samp << "\t" << value << "\n";
			   
			}
		     }
		  }
	       }
	    }
	 }
      }
      PedFile.close();
   }
   
   
   
}
