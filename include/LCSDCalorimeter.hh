#ifndef LCSDCalorimeter_h
#define LCSDCalorimeter_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHitCalorimeter.hh"

class G4Step;
class G4HCofThisEvent;

class LCSDCalorimeter : public G4VSensitiveDetector{
    public:
        LCSDCalorimeter(G4String name);
        virtual ~LCSDCalorimeter();

        virtual void Initialize(G4HCofThisEvent *HCE);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCCalorimeterHC *fCalHC;
        G4int fCalHCID;
};

#endif
