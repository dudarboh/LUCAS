#ifndef LCSDTracker_h
#define LCSDTracker_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHitTracker.hh"

class G4Step;
class G4HCofThisEvent;

class LCSDTracker : public G4VSensitiveDetector{
    public:
        LCSDTracker(G4String name);
        virtual ~LCSDTracker();

        virtual void Initialize(G4HCofThisEvent *HCE);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCTrackerHC *fTrackerHC;
        G4int fTrackerHCID;
};

#endif
