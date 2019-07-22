/*
 * LCRootOut.hh
 *
 *  Created on: Apr 23, 2009
 *      Author: aguilar
 *
 *      Fill a Tree object, then write the object to file
 *      You can get help from the ROOT documentation at http://root.cern.ch
 *
 *      MAJOR question: is there going to be just one tree,
 *      or one tree for each hit?
 */

#ifndef LCROOTOUT_HH_
#define LCROOTOUT_HH_ 1

#include "LCSensitiveDetector.hh"
#include "LCHit.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

// root classes:
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"

#include "Track_t.hh"
#include "Hit_t.hh"

// namespaces
using namespace std;
using namespace TMath;

// ------------------------------------------------------------------
// ROOT output class
// ------------------------------------------------------------------

class LCRootOut
{
public:
    LCRootOut();
    LCRootOut(const G4String fName );
   ~LCRootOut();

public:
 
  void Init();                                                         // opens a file, creates a Tree 
  void ProcessEvent(const G4Event* event, LCHitsCollection *HitsColl);
  void ProcEventAccumulate( LCHitsCollection *HitsColl);
  void End();                                                         // writes to file and closes it
  void SetAddresses();                                                // sets branch addresses in "UPDATE" mode
  void CreateNewTree();                                               // creates new Tree
  TFile *GetFile(){ return _file; }
  // root variables:
static TFile *pRootFile;


private:
  // root output file name 
  G4String RootOutFile;
  TFile *_file;
  TTree *_LcalData;
  //
   G4double _z0;
   G4double _dz;
   G4double _r0;
   G4double _dr;
   G4double _phi0;
   G4double _phiOffset;
   G4double _dphi;
  //
   vector<G4int> theUsedCells;
public:
 

private:
  G4double vX, vY, vZ;
  G4int numPrim;       // number of primary particles
  vector< Track_t > Tracks;
  vector< Track_t > *pTracks;
  G4int numHits;     // total number of hits
  //  caloHit
  vector< Hit_t > Hits;
  vector< Hit_t > *pHits;
  G4double Etot[2];       // total energy deposit in arm per arm
  G4double Emax;          // max  energy deposit in cell
//------------------------------------------------------------------------

};

#endif /* LCROOTOUT_HH_ */
