#ifndef LCEventAction_h
#define LCEventAction_h 1

#include "G4UserEventAction.hh"
#include "LCHit.hh"

class LCEventAction:public G4UserEventAction{
    public:
        LCEventAction();
        virtual ~LCEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);
};

#endif