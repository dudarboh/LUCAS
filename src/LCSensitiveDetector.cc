#include "LCSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSensitiveDetector::LCSensitiveDetector(const G4String& name,
                            const G4String& hitsCollectionName)
    :G4VSensitiveDetector(name),
    fHitsCollection(nullptr){
    collectionName.insert(hitsCollectionName);
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *hce){
    fHitsCollection = new LCHitsCollection(SensitiveDetectorName, collectionName[0]);

    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
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
    if(!hit){
        G4ExceptionDescription msg;
        msg << "Cannot access hit " << "Sector: "<<sector<<" Pad: "<<pad<<" Layer: "<<layer; 
        G4Exception("LCSensitiveDetector::ProcessHits()", "no hit for this cell in the collection", FatalException, msg);
    }
    hit->AddHitEnergy(step_energy);

    if(prePoint->GetStepStatus() == fGeomBoundary){
        G4double pz = (step->GetTrack()->GetMomentum()).getZ();
        if(pz > 0.){
            hit->AddDirParticle();
        }
        else{
            hit->AddBSParticle();
        }
    }

    return true;
}


