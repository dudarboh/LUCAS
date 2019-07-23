#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "LCEventAction.hh"
#include "globals.hh"

class SteppingAction : public G4UserSteppingAction{
    public:
        SteppingAction(LCEventAction*);
        virtual ~SteppingAction(){};
        virtual void UserSteppingAction(const G4Step* aStep);

    private:
        LCEventAction *eventaction;
        G4int MaxStepNumber, oldPID;
        G4double StepSizeMin;
        G4int noZeroSteps;
        G4int noZeroStepsTotal;
        G4bool LastStepZero, ThisStepZero;

};

#endif
