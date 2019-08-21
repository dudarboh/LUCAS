#include "LCSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSensitiveDetector::LCSensitiveDetector(const G4String& name,
                            const G4String& hitsCollectionName, 
                            G4double rho_min,
                            G4double pad_rho_width,
                            G4double pad_phi_width)
    :G4VSensitiveDetector(name),
    fHitsCollection(nullptr),
    fRhoMin(rho_min),
    fPadRhoWidth(pad_rho_width),
    fPadPhiWidth(pad_phi_width){
    collectionName.insert(hitsCollectionName);
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *hce){
    // Create hits collection
    fHitsCollection = new LCHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Add this collection to HCofThisEvent
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
    // Create hits for each calorimeter cell
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

    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4ThreeVector GlobalHitPos = preStepPoint->GetPosition();
    G4ThreeVector LocalHitPos = preStepPoint->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);

    // Sector, pad, layer of hit
    G4int sector = floor((LocalHitPos.getPhi() - 5./12. * M_PI) / fPadPhiWidth);
    if(sector == 4) sector = 3;
    G4int pad = floor((LocalHitPos.getRho() - fRhoMin) / fPadRhoWidth);
    if(pad == 64) pad = 63;
    G4int layer = preStepPoint->GetTouchableHandle()->GetCopyNumber(1);
    
    G4int cellID = pad + 64 * sector + 256 * layer;

    LCHit *hit = (*fHitsCollection)[cellID];
    if(!hit){
        G4ExceptionDescription msg;
        msg << "Cannot access hit " << "Sector: "<<sector<<" Pad: "<<pad<<" Layer: "<<layer; 
        G4Exception("LCSensitiveDetector::ProcessHits()", "no hit for this cell in the collection", FatalException, msg);
    }
    hit->AddHitEnergy(step_energy);

    // If this step by Transportation process (Cross the boundary)
    // If this step goes in, not out (Check the VolumeName)

    const G4VProcess* currentProcess=preStepPoint->GetProcessDefinedStep();
    if(currentProcess != 0){
        const G4String & stepProcessName = currentProcess->GetProcessName();
        G4String volumePos = step->GetTrack()->GetNextVolume()->GetName();
        if(stepProcessName == "Transportation" && volumePos == "Si"){
            G4double pz = (step->GetTrack()->GetMomentum()).getZ();
            if(pz > 0.) hit->AddDirParticle();
            else hit->AddBSParticle();
        }
    }

    return true;
}


