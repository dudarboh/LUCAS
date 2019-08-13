#include "LCSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"

LCSensitiveDetector::LCSensitiveDetector(const G4String& name,
                            const G4String& hitsCollectionName, 
                            G4double rho_min,
                            G4double pad_rho_width,
                            G4double pad_phi_width,
                            G4int n_pads)
    :G4VSensitiveDetector(name),
    fHitsCollection(nullptr),
    fRhoMin(rho_min),
    fPadRhoWidth(pad_rho_width),
    fPadPhiWidth(pad_phi_width),
    fNPads(n_pads){
    collectionName.insert(hitsCollectionName);
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *hce){
    fHitsCollection = new LCHitsCollection(SensitiveDetectorName, collectionName[0]);
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool LCSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory*){
    G4double step_energy = step->GetTotalEnergyDeposit();
    if(step_energy == 0.) return false;

    G4ThreeVector GlobalHitPos = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector LocalHitPos = step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);
    // Sector, pad, layer of hit
    G4int sector = floor((LocalHitPos.getPhi() - 5./12. * M_PI) / fPadPhiWidth);
    if(sector == 4) sector = 3;
    G4int pad = floor((LocalHitPos.getRho() - fRhoMin) / fPadRhoWidth);
    G4int layer = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
    // Check if hit exists, if not create a new one
    
    LCHit *processed_hit;
    LCHit *existing_hit;
    G4bool isExist = false;
    for(G4int i=0; i<fHitsCollection->entries(); i++){
        existing_hit = (*fHitsCollection)[i];
        //If hit is found
        if(sector == existing_hit->GetSector() && pad == existing_hit->GetPad() && layer == existing_hit->GetLayer()){
            processed_hit = existing_hit;
            isExist=true;
            break;
        }
    }

    if(!isExist) processed_hit = new LCHit(sector, pad, layer);
    processed_hit->AddHitEnergy(step_energy);

    // Check only trackers. Because its a mess in calorimeter
    if((layer == 0 || layer == 1) && processed_hit->GetBS() == -999){
        if(step->GetTrack()->GetMomentum().getZ() > 0.) processed_hit->SetBS(0);
        else processed_hit->SetBS(1);
    }

    if(!isExist) fHitsCollection->insert(processed_hit);
    return true;
}
