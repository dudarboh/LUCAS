#ifndef LCSDTrigger_h
#define LCSDTrigger_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHitTrigger.hh"

class G4Step;
class G4HCofThisEvent;

class LCSDTrigger : public G4VSensitiveDetector{
    public:
        LCSDTrigger(G4String name);
        virtual ~LCSDTrigger();

        virtual void Initialize(G4HCofThisEvent *HCE);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCTriggerHC *fTriggerHC;
        G4int fTriggerHCID;
};

#endif
