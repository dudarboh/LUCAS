#ifndef LCRunAction_h
#define LCRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"

class LCEventAction;

class G4Run;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction(LCEventAction* eventAction, G4String OutputFileName);
        virtual ~LCRunAction();

        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction (const G4Run*);
    private:
      LCEventAction* fEventAction;
      G4String fOutputFileName;
};

#endif
