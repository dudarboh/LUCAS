#include "LCSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSensitiveDetector::LCSensitiveDetector(G4String detector_name)
    :G4VSensitiveDetector(detector_name){
    collectionName.insert("LumiCalHitsCollection");
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *HCE){
    fHitsCollection = new LCHitsCollection(SensitiveDetectorName, collectionName[0]);
    HCE->AddHitsCollection(GetCollectionID(0), fHitsCollection);

    for(G4int layer=0; layer<8; layer++){
        for(G4int sector=0; sector<4; sector++){
            for(G4int pad=0; pad<64; pad++){
                fHitsCollection->insert(new LCHit(sector, pad, layer));
            }
        }
    }
}

G4bool LCSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory*){

    G4double step_energy = step->GetTotalEnergyDeposit();
    if(step_energy == 0.) return false;

    G4StepPoint* prePoint = step->GetPreStepPoint();

    G4int pad = prePoint->GetTouchable()->GetReplicaNumber(0);
    G4int sector = prePoint->GetTouchable()->GetReplicaNumber(1);
    //Carbon fiber copy number
    G4int layer = prePoint->GetTouchable()->GetCopyNumber(3);

    G4int cellID = pad + 64 * sector + 256 * layer;

    LCHit *hit = (*fHitsCollection)[cellID];

    hit->AddHitEnergy(step_energy);

    if(hit->GetIsPrimary() == 0 && step->GetTrack()->GetTrackID() == 1){
        hit->SetIsPrimary();
    }

    return true;
}


