
#include "LCRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <sys/times.h>

LCRunAction::LCRunAction(LCRootOut *RO){fRootOut = RO;}

LCRunAction::LCRunAction(){fRootOut = 0;}

LCRunAction::~LCRunAction(){;}

void LCRunAction::BeginOfRunAction(const G4Run*){
    tms fTimeNow;
    clock_t StartTime = times(&fTimeNow);
    G4long seed = StartTime;
    G4RandGauss::setTheSeed(seed);

    if(fRootOut) fRootOut->Init();
}

void LCRunAction::EndOfRunAction(const G4Run*){
    if(fRootOut) fRootOut->End();
}
