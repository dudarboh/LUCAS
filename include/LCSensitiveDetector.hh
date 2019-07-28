/*
 *      Borrows heavily from Geant4 novice example 4 (ExN04)
 *      and LumiCalSD, written by Bogdan Pawlik
 */

#ifndef LCSENSETIVEDETECTOR_HH_
#define LCSENSETIVEDETECTOR_HH_ 1

#include "LCHit.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;


#include "G4TouchableHandle.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NavigationHistory.hh"
#include "G4PVReplica.hh"

typedef std::map<G4int,LCHit*> LCHitMap;
typedef LCHitMap::iterator LCHitMapIterator;
typedef std::pair<G4int,LCHit*> LCHitMapPair;


class LCSensitiveDetector : public G4VSensitiveDetector{
    public:
        LCSensitiveDetector(G4String sdname,
                            G4double rmin,
                            G4double phi0,
                            G4double cRho,
                            G4double cPhi,
                            G4int nCellRho,
                            G4int nCellPhi,
                            G4bool cellvirtual);
        ~LCSensitiveDetector();

        void Initialize(G4HCofThisEvent *HCE);
        G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
        void EndOfEvent(G4HCofThisEvent *HCE);

    private:
        G4int fCollectionID;
        G4String fSDName;

        LCHitsCollection *fHitsCollection;

        G4bool fVirtualCell;
        G4int fNumCellsRho, fNumSectorsPhi;
        G4double fCellDimRho, fCellDimPhi;
        G4double fCalRhoMin, fCalPhiOffset;

        G4int fPrimaryID;
        G4int fPrimaryPDG;


        // Set methods for SD primary parameters - the volumes that make up LC
        void SetNCellPhi(G4int nx);
        void SetNCellRho(G4int ny);
        void SetRhoCellDim(G4double c1);
        void SetPhiCellDim(G4double c2);
        void SetRhoMin(G4double c);
        void SetPhiOffset(G4double phi);

        // find if hit exists already and ++edep if it does
        // don't think I need TID
        G4bool FindHit(G4int cellid, G4double dE);

        // map of hits
        LCHitMap *hitMap;

};

#endif