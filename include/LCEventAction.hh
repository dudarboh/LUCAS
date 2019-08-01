#ifndef LCEVENTACTION_HH_
#define LCEVENTACTION_HH_ 1

#include "G4UserEventAction.hh"
#include "LCEventData.hh"

class LCEventAction:public G4UserEventAction{
    public:
        LCEventAction();
        LCEventAction(LCEventData *EventData);
        virtual ~LCEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

    private:
        G4int fHCID; // ID number for a collection of hits
        LCEventData *fEventData;
};

#endif