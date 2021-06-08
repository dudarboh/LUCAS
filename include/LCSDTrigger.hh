#ifndef LCSDTrigger_h
#define LCSDTrigger_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHitTrigger.hh"

class G4Step;
class G4HCofThisEvent;

class LCSDTrigger : public G4VSensitiveDetector{
    public:
        LCSDTrigger(const G4String& name, const G4String& hitsCollectionName);
        virtual ~LCSDTrigger();

        virtual void Initialize(G4HCofThisEvent *hce);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCTriggerHC *fHitsCollection;
};

#endif
