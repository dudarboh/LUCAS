#ifndef LCSensitiveDetector_h
#define LCSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "LCHit.hh"

class G4Step;
class G4HCofThisEvent;

class LCSensitiveDetector : public G4VSensitiveDetector{
    public:
        LCSensitiveDetector(const G4String& name,
                            const G4String& hitsCollectionName, 
                            G4double rho_min,
                            G4double pad_rho_width,
                            G4double pad_phi_width);
        virtual ~LCSensitiveDetector();

        virtual void Initialize(G4HCofThisEvent *hitCollection);
        virtual G4bool ProcessHits(G4Step *step, G4TouchableHistory *history);

    private:
        LCHitsCollection *fHitsCollection;

        G4double fRhoMin;
        G4double fPadRhoWidth, fPadPhiWidth;
};

#endif