#include "LCEventAction.hh"
#include "LCRunAction.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"

LCEventAction::LCEventAction():fHCID(-1){fEventData = 0;}
LCEventAction::LCEventAction(LCEventData *EventData):fHCID(-1){fEventData = EventData;}

LCEventAction::~LCEventAction(){;}

void LCEventAction::BeginOfEventAction(const G4Event* event){
    if(event->GetEventID() % 50 == 0){
        G4cout<<"Event number: "<<event->GetEventID()<<G4endl;
        std::chrono::duration<double> timeElapsed = std::chrono::system_clock::now() - startTime;
        G4cout<<"Time elapsed: "<<timeElapsed.count()<<" sec"<<G4endl;
    }
}

void LCEventAction::EndOfEventAction(const G4Event* event){
    // Use a Sensitive Detector manager to assign an ID # to the hits collections
    if(fHCID<0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LCHitsCollectionName");
    if(fHCID<0) G4cerr<<"No HitCollections with name "<<"LCHitsCollectionName"<< " are found"<<G4endl;

    LCHitsCollection *HitsCollection = (LCHitsCollection*)(event->GetHCofThisEvent()->GetHC(fHCID));

    // fill the ROOT Tree Analysis manager
    if(HitsCollection && fEventData){
        fEventData->FillEventData(event, HitsCollection);
        fEventData->Clear();
    }
}