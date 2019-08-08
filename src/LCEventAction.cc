#include "LCEventAction.hh"
#include "LCSensitiveDetector.hh"
#include "LCHit.hh"
#include "LCRunAction.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"
#include "g4root.hh"

LCEventAction::LCEventAction(LCRunAction *runAction)
    :G4UserEventAction(),
    fHCID(-1){fRunAction = runAction;}

LCEventAction::~LCEventAction(){;}

LCHitsCollection* LCEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const{
    auto hitsCollection = static_cast<LCHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
    if(!hitsCollection){
        G4ExceptionDescription msg;
        msg<<"Cannot access hitsCollection ID"<<hcID;
        G4Exception("LCEventAction::GetHitsCollection()", "3:", FatalException, msg);
    }
    return hitsCollection;
}

void LCEventAction::BeginOfEventAction(const G4Event* event){
    if(event->GetEventID() % 100 == 0){
        G4cout<<"Event number: "<<event->GetEventID()<<G4endl;
        std::chrono::duration<double> timeElapsed = std::chrono::system_clock::now() - launchTime;
        G4cout<<"Time elapsed: "<<timeElapsed.count()<<" sec"<<G4endl;
    }
}

void LCEventAction::EndOfEventAction(const G4Event* event){
    if(fHCID<0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LumiCalHitsCollection");
    LCHitsCollection *HitsCollection = GetHitsCollection(fHCID, event);

    if(HitsCollection && fRunAction){
        fRunAction->FillEventData(event, HitsCollection);
        fRunAction->ClearVectors();
    }
}
