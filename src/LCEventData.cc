#include "LCEventData.hh"

#include "g4root.hh"

LCEventData::LCEventData(){;}

LCEventData::~LCEventData(){;}

void LCEventData::FillEventData(const G4Event* event, LCHitsCollection *HitsCollection){
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

    G4int hit_n = HitsCollection->entries();
    analysisManager->FillNtupleIColumn(6, hit_n);

    LCHit *hit;

    for(G4int i=0; i<hit_n; i++){
        hit = (*HitsCollection)[i];
        hit_sector.push_back(hit->hit_sector);
        hit_pad.push_back(hit->hit_pad);
        hit_layer.push_back(hit->hit_layer);
        hit_energy.push_back(hit->hit_energy);
        hit_bs.push_back(hit->hit_bs);
    }
    analysisManager->AddNtupleRow();
}

void LCEventData::Clear(){
    hit_sector.clear();
    hit_pad.clear();
    hit_layer.clear();
    hit_energy.clear();
    hit_bs.clear();
}