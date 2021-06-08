#ifndef LCSD_h
#define LCSD_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHit.hh"

class G4Step;
class G4HCofThisEvent;

class LCSD : public G4VSensitiveDetector{
    public:
        LCSD(const G4String& name, const G4String& hitsCollectionName);
        virtual ~LCSD(){};

        virtual void Initialize(G4HCofThisEvent *hce);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);
        void RecordEnergyDeposition(G4Step *step, LCHit* hit, double weight);

    private:
        LumicalHC *fHitsCollection;
};

#endif
