#ifndef LCEventAction_h
#define LCEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

class LCEventAction : public G4UserEventAction{
    public:
        LCEventAction();
        virtual ~LCEventAction();

        virtual void  BeginOfEventAction(const G4Event* event);
        virtual void    EndOfEventAction(const G4Event* event);
        void ClearVectors();

        G4int fCalHCID;
        G4int fTracker1HCID;
        G4int fTracker2HCID;
        G4int fTriggerHCID;

        std::vector<G4int> fCalSector;
        std::vector<G4int> fCalPad;
        std::vector<G4int> fCalLayer;
        std::vector<G4double> fCalEnergy;

        std::vector<G4int> fTr1Sector;
        std::vector<G4int> fTr1Pad;
        std::vector<G4int> fTr1Layer;
        std::vector<G4double> fTr1Energy;
        std::vector<G4int> fTr1Type;
        std::vector<G4double> fTr1TrackLen;
        std::vector<G4double> fTr1X;
        std::vector<G4double> fTr1Y;
        std::vector<G4double> fTr1Z;
        std::vector<G4double> fTr1Px;
        std::vector<G4double> fTr1Py;
        std::vector<G4double> fTr1Pz;
        std::vector<G4double> fTr1PEnergy;

        std::vector<G4int> fTr2Sector;
        std::vector<G4int> fTr2Pad;
        std::vector<G4int> fTr2Layer;
        std::vector<G4double> fTr2Energy;
        std::vector<G4int> fTr2Type;
        std::vector<G4double> fTr2TrackLen;
        std::vector<G4double> fTr2X;
        std::vector<G4double> fTr2Y;
        std::vector<G4double> fTr2Z;
        std::vector<G4double> fTr2Px;
        std::vector<G4double> fTr2Py;
        std::vector<G4double> fTr2Pz;
        std::vector<G4double> fTr2PEnergy;

};

#endif
