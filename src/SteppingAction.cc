#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "LCEventAction.hh"
#include "Setup.hh"

#include "G4SystemOfUnits.hh"


SteppingAction::SteppingAction(LCEventAction* evt):eventaction(evt){}

void SteppingAction::UserSteppingAction(const G4Step*){}
