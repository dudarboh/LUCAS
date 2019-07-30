#ifndef LCEVENTACTION_HH_
#define LCEVENTACTION_HH_ 1

#include "G4UserEventAction.hh"
#include "LCRootOut.hh"

class LCEventAction : public G4UserEventAction{
    public:
        LCEventAction(LCRootOut *RO);
        LCEventAction();
        virtual ~LCEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

    private:
        LCHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;

        G4int fHCID; // ID number for a collection of calorimeter hits
 
        LCRootOut *fRootOut; // Handles writing the ROOT tree
};

#endif