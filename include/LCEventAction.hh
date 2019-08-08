#ifndef LCEventAction_h
#define LCEventAction_h 1

#include "G4UserEventAction.hh"
#include "LCHit.hh"
#include "LCRunAction.hh"

class LCEventAction:public G4UserEventAction{
    public:
        LCEventAction(LCRunAction *runAction);
        virtual ~LCEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

    private:
        G4int fHCID; // ID number for a collection of hits
        LCHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;
        LCRunAction *fRunAction;
};

#endif