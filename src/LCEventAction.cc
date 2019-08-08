#include "LCEventAction.hh"
#include "LCRunAction.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"

LCEventAction::LCEventAction():fHCID(-1){fEventData = 0;}
LCEventAction::LCEventAction(LCEventData *EventData):fHCID(-1){fEventData = EventData;}

LCEventAction::~LCEventAction(){;}

void LCEventAction::BeginOfEventAction(const G4Event* event){
    if(event->GetEventID() % 100 == 0){
        G4cout<<"Event number: "<<event->GetEventID()<<G4endl;
        std::chrono::duration<double> timeElapsed = std::chrono::system_clock::now() - launchTime;
        G4cout<<"Time elapsed: "<<timeElapsed.count()<<" sec"<<G4endl;
    }
}

void LCEventAction::EndOfEventAction(const G4Event* event){
    if(fHCID<0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LCHitsCollectionName");
    LCHitsCollection *HitsCollection = (LCHitsCollection*)(event->GetHCofThisEvent()->GetHC(fHCID));

    if(HitsCollection && fEventData){
        fEventData->FillEventData(event, HitsCollection);
        fEventData->Clear();
    }
}