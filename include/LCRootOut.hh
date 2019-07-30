#ifndef LCROOTOUT_HH_
#define LCROOTOUT_HH_ 1

#include "TFile.h"
#include "TTree.h"

#include "LCDetectorConstruction.hh"
#include "G4Event.hh"

class LCRootOut{
    public:
        LCRootOut();
        ~LCRootOut();
 
        void Init(); // opens a file, creates a Tree
        void ProcessEvent(const G4Event* event, LCHitsCollection *HitsColl);
        void End(); // writes to file and closes it
        void CreateNewTree(); // creates new Tree

    private:
        TFile *_file;
        TTree *_LcalData;

        G4double vX, vY, vZ;

        G4int Track_N;
        G4double Track_px[999];
        G4double Track_py[999];
        G4double Track_pz[999];

        G4int Hit_N;
        G4double Hit_energy[2500];
        G4int Hit_xCell[2500];
        G4int Hit_yCell[2500];
        G4int Hit_zCell[2500];
        G4double Hit_xHit[2500];
        G4double Hit_yHit[2500];
        G4double Hit_zHit[2500];

        G4double Emax; // max  energy deposit in cell
};

#endif
