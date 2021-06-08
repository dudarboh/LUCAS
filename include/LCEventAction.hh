#ifndef LCEventAction_h
#define LCEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class LCEventAction : public G4UserEventAction{
    public:
        LCEventAction();
        virtual ~LCEventAction(){};

        virtual void  BeginOfEventAction(const G4Event*){};
        virtual void    EndOfEventAction(const G4Event* event);
        void ClearVectors();

        G4int fHCID;
        G4int fTriggerHCID;

        std::vector<G4int> fSector;
        std::vector<G4int> fPad;
        std::vector<G4int> fLayer;
        std::vector<G4int> fIsPrimary;
        std::vector<G4double> fTotalEdep;
        std::vector<G4double> fTotalTrackLen;
        std::vector<G4int> fMcContId;
        std::vector<G4double> fMcContPosX;
        std::vector<G4double> fMcContPosY;
        std::vector<G4double> fMcContPosZ;
        std::vector<G4double> fMcContMomX;
        std::vector<G4double> fMcContMomY;
        std::vector<G4double> fMcContMomZ;
        std::vector<G4double> fMcContEnergy;
        std::vector<G4double> fMcContEdep;
        std::vector<G4double> fMcContTrackLen;
        std::vector<G4int> fMcContPdg;
};

#endif
