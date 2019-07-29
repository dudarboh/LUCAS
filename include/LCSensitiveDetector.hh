/*
 *      Borrows heavily from Geant4 novice example 4 (ExN04)
 *      and LumiCalSD, written by Bogdan Pawlik
 */

#ifndef LCSENSETIVEDETECTOR_HH_
#define LCSENSETIVEDETECTOR_HH_ 1

#include "G4VSensitiveDetector.hh"
#include "LCHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

class LCSensitiveDetector : public G4VSensitiveDetector{
    public:
        LCSensitiveDetector(const G4String& name,
                            const G4String& hitsCollectionName, 
                            G4double rmin,
                            G4double cRho,
                            G4double cPhi,
                            G4int nCellRho);
        virtual ~LCSensitiveDetector();

        virtual void Initialize(G4HCofThisEvent *hitCollection);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);
        virtual void EndOfEvent(G4HCofThisEvent *hitCollection);

    private:
        LCHitsCollection *fHitsCollection;

        G4double fCalRhoMin;
        G4double fCellDimRho, fCellDimPhi;
        G4int fNumCellsRho;

};

#endif