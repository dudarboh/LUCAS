#ifndef LCSensitiveDetector_h
#define LCSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHit.hh"

class G4Step;
class G4HCofThisEvent;

class LCSensitiveDetector : public G4VSensitiveDetector{
    public:
        LCSensitiveDetector(G4String name);
        virtual ~LCSensitiveDetector();

        virtual void Initialize(G4HCofThisEvent *hitCollection);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCHitsCollection *fHitsCollection;
};

#endif