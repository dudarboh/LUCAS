#include "LCEventAction.hh"
#include "LCSensitiveDetector.hh"
#include "LCHit.hh"
#include "LCRunAction.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"
#include "g4root.hh"

LCEventAction::LCEventAction():G4UserEventAction(){;}

LCEventAction::~LCEventAction(){;}

void LCEventAction::BeginOfEventAction(const G4Event* event){
    //Count how much time elapsed
    if(event->GetEventID() % 1000 == 0){
        std::chrono::duration<double> timeElapsed = std::chrono::system_clock::now() - launchTime;
        G4cout<<"Time elapsed: "<<timeElapsed.count()<<" sec"<<G4endl;
    }
}

void LCEventAction::EndOfEventAction(const G4Event*){;}
