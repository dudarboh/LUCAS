#include "LCSDCalorimeter.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSDCalorimeter::LCSDCalorimeter(G4String detector_name)
:G4VSensitiveDetector(detector_name),
fCalHCID(-1){
    collectionName.insert("LCCalHC");
}

LCSDCalorimeter::~LCSDCalorimeter(){}

void LCSDCalorimeter::Initialize(G4HCofThisEvent *HCE){
    std::cout<<"Start of LCSDCalorimeter::Initialize"<<std::endl;

    fCalHC = new LCCalorimeterHC(SensitiveDetectorName, collectionName[0]);
    if (fCalHCID<0) fCalHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fCalHC);
    HCE->AddHitsCollection(fCalHCID, fCalHC);

    // Create LCHit object for each pad position and add it to the hit collection.
    // Hit collection is created at the begining of each event
    for(G4int layer=2; layer<8; layer++){
        for(G4int sector=0; sector<4; sector++){
            for(G4int pad=0; pad<64; pad++){
                fCalHC->insert(new LCHitCalorimeter(sector, pad, layer));
            }
        }
    }
    std::cout<<"End of LCSDCalorimeter::Initialize"<<std::endl;
}

G4bool LCSDCalorimeter::ProcessHits(G4Step *step, G4TouchableHistory*){
    // std::cout<<"Start of LCSDCalorimeter::ProcessHits"<<std::endl;

    G4double e_dep = step->GetTotalEnergyDeposit();
    if (e_dep <= 0.) return true;

    auto prePoint = step->GetPreStepPoint();
    auto touchable = prePoint->GetTouchable();

    G4int sector = touchable->GetReplicaNumber(1);
    G4int pad = touchable->GetReplicaNumber(0);
    G4int layer = touchable->GetCopyNumber(3);

    G4int cellID = pad + 64 * sector + 256 * (layer-2);
    LCHitCalorimeter *hit = (*fCalHC)[cellID];

    hit->energy += e_dep;

    // std::cout<<"End of LCSDCalorimeter::ProcessHits"<<std::endl;
    return true;
}
