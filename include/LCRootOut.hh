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

#include "LCDetectorConstruction.hh"

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

// namespaces
using namespace std;
using namespace TMath;

// ROOT output class
class LCRootOut{
    public:
        LCRootOut();
        LCRootOut(const G4String fName);
        ~LCRootOut();
 
        void Init(); // opens a file, creates a Tree
        void ProcessEvent(const G4Event* event, LCHitsCollection *HitsColl);
        void ProcEventAccumulate(LCHitsCollection *HitsColl);
        void End(); // writes to file and closes it
        void SetAddresses(); // sets branch addresses in "UPDATE" mode
        void CreateNewTree(); // creates new Tree
        TFile *GetFile(){return _file;}
        
        // root variables:
        static TFile *pRootFile;

    private:
        // root output file name 
        G4String RootOutFile;
        TFile *_file;
        TTree *_LcalData;

        G4double _z0;
        G4double _dz;
        G4double _r0;
        G4double _dr;
        G4double _phi0;
        G4double _phiOffset;
        G4double _dphi;

        vector<G4int> theUsedCells;

        G4double vX, vY, vZ;

        G4int Track_N;
        G4double Track_px[999];
        G4double Track_py[999];
        G4double Track_pz[999];
        G4int Track_ID[999];
        G4int Track_PDG[999];

        G4int Hit_N;
        G4int Hit_cellID[2500];
        G4double Hit_energy[2500];
        G4double Hit_xCell[2500];
        G4double Hit_yCell[2500];
        G4double Hit_zCell[2500];
        G4double Hit_xHit[2500];
        G4double Hit_yHit[2500];
        G4double Hit_zHit[2500];
        G4double Hit_TOF[2500];

        G4double Emax; // max  energy deposit in cell
};

#endif
