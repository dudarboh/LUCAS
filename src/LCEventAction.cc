#include "LCEventAction.hh"

#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4RootAnalysisManager.hh"
#include "g4root.hh"

LCEventAction::LCEventAction(G4bool rootOutput) : fHCID(-1), fRootOutput(rootOutput){}


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
    if(HitsCollection && fRootOutput){
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        G4PrimaryVertex *pVertex = event->GetPrimaryVertex(0);
        G4double prim_x = pVertex->GetX0();
        G4double prim_y = pVertex->GetY0();
        G4double prim_z = pVertex->GetZ0();

        G4PrimaryParticle *pParticle = pVertex->GetPrimary();
        G4double prim_px = (pParticle->GetMomentum()).getX();
        G4double prim_py = (pParticle->GetMomentum()).getY();
        G4double prim_pz = (pParticle->GetMomentum()).getZ();

        analysisManager->FillNtupleDColumn(0, prim_x);
        analysisManager->FillNtupleDColumn(1, prim_y);
        analysisManager->FillNtupleDColumn(2, prim_z);

        analysisManager->FillNtupleDColumn(3, prim_px);
        analysisManager->FillNtupleDColumn(4, prim_py);
        analysisManager->FillNtupleDColumn(5, prim_pz);

        G4cout<<"Collection events: "<<HitsCollection->entries()<<G4endl;

        G4int hit_n = HitsCollection->entries();
        analysisManager->FillNtupleIColumn(6, hit_n);

        if(hit_n > 2500) G4Exception("Number of hits ", "hit_n", RunMustBeAborted," exceed array size of 2500!");        

        for(G4int i=0; i<hit_n; i++){
            analysisManager->FillNtupleDColumn(7, (*HitsCollection)[i]->GetX());
            analysisManager->FillNtupleDColumn(8, (*HitsCollection)[i]->GetY());
            analysisManager->FillNtupleDColumn(9, (*HitsCollection)[i]->GetZ());

            analysisManager->FillNtupleIColumn(10, (*HitsCollection)[i]->GetSector());
            analysisManager->FillNtupleIColumn(11, (*HitsCollection)[i]->GetPad());
            analysisManager->FillNtupleIColumn(12, (*HitsCollection)[i]->GetLayer());

            analysisManager->FillNtupleDColumn(13, (*HitsCollection)[i]->GetEnergy());
            analysisManager->FillNtupleIColumn(14, (*HitsCollection)[i]->GetDirection());
            analysisManager->FillNtupleIColumn(15, (*HitsCollection)[i]->GetBornInSi());
            analysisManager->FillNtupleIColumn(16, (*HitsCollection)[i]->GetPDG());
        }
        analysisManager->AddNtupleRow();
    }
}