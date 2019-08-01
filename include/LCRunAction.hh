#ifndef LCRUNACTION_HH_
#define LCRUNACTION_HH_ 1

#include "G4UserRunAction.hh"
#include "LCEventData.hh"
#include "G4Run.hh"

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction();
        LCRunAction(LCEventData *EventData);
        ~LCRunAction();

        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction (const G4Run*);

        G4bool fEventData;
};

#endif
