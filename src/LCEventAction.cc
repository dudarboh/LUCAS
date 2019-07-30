#include "LCEventAction.hh"
#include "LCRootOut.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"

LCEventAction::LCEventAction(LCRootOut *RO) : fHCID(-1){
    fRootOut = RO;
}

LCEventAction::LCEventAction() : fHCID(-1), fRootOut(0){}

LCEventAction::~LCEventAction(){}

LCHitsCollection* LCEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const{
    auto hitsCollection = static_cast<LCHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
    
    if(!hitsCollection){
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID; 
        G4Exception("B4cEventAction::GetHitsCollection()",
        "Meow >:3", FatalException, msg);
    }         

    return hitsCollection;
}

void LCEventAction::BeginOfEventAction(const G4Event*){}


void LCEventAction::EndOfEventAction(const G4Event* event){
    // Use a Sensitive Detector manager to assign an ID # to the hits collections
    if(fHCID<0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LCHitsCollectionName");
    if(fHCID<0) G4cerr<<"No HitCollections with name "<<"LCHitsCollectionName"<< " are found"<<G4endl;

    LCHitsCollection *HitsCollection = GetHitsCollection(fHCID, event);

    // fill the ROOT Tree
    if(HitsCollection && fRootOut) fRootOut->ProcessEvent(event, HitsCollection);
}