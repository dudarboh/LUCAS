#include "LCSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSD::LCSD(const G4String& name, const G4String& hitsCollectionName)
:G4VSensitiveDetector(name),
fHitsCollection(nullptr){
    collectionName.insert(hitsCollectionName);
}

void LCSD::Initialize(G4HCofThisEvent *hce){
    // std::cout<<"Start of LCSD::Initialize"<<std::endl;
    // Create hits collection
    fHitsCollection = new LumicalHC(SensitiveDetectorName, collectionName[0]);

    // Add this collection in hce
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection( hcID, fHitsCollection );

    // Create LCHit object for each pad position and add it to the hit collection.
    // Hit collection is created at the begining of each event
    for(G4int layer=0; layer<8; layer++){
        for(G4int sector=0; sector<4; sector++){
            for(G4int pad=0; pad<64; pad++){
                fHitsCollection->insert(new LCHit(sector, pad, layer));
            }
        }
    }

    // std::cout<<"End of LCSD::Initialize"<<std::endl;
}

G4bool LCSD::ProcessHits(G4Step *step, G4TouchableHistory*){
    // std::cout<<"Start of LCSD::ProcessHits"<<std::endl;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep <= 0.) return true;

    auto prePoint = step->GetPreStepPoint();
    G4ThreeVector pos = prePoint->GetPosition();

    auto touchable = prePoint->GetTouchable();
    G4int sector = touchable->GetReplicaNumber(1);
    G4int pad = touchable->GetReplicaNumber(0);
    G4int layer = touchable->GetCopyNumber(3);
    G4int cellID = pad + 64 * sector + 256 * layer;

    G4double localX = touchable->GetHistory()->GetTopTransform().TransformPoint(pos).x();
    G4double localY = touchable->GetHistory()->GetTopTransform().TransformPoint(pos).y();

    G4double r = std::hypot(localX, localY);
    G4double phi = std::atan2(localY, localX);
    //Distance to the shortest side
    G4double dAzimutal = r*std::sin( 3.75/180.*M_PI - std::abs(phi));
    G4double dRadial = 0.9 - std::abs(r - (80. + 0.9 + 1.8*pad) );

    auto hit = (*fHitsCollection)[cellID];
    if (dRadial < 0.3 || dAzimutal < 0.3){
        // radial pad is closest
        if (dRadial < dAzimutal){
            G4double fraction = 0.5 + 0.5/0.3*dRadial;
            RecordEnergyDeposition(step, hit, fraction);
            // top side
            if (r - (80. + 0.9 + 1.8*pad) > 0 && pad != 63){
                auto hitNeighbor = (*fHitsCollection)[cellID + 1];
                RecordEnergyDeposition(step, hitNeighbor, 1. - fraction);
            }
            // bot side
            else if (r - (80. + 0.9 + 1.8*pad) < 0 && pad != 0){
                auto hitNeighbor = (*fHitsCollection)[cellID - 1];
                RecordEnergyDeposition(step, hitNeighbor, 1. - fraction);
            }
        }
        else{
            G4double fraction = 0.5 + 0.5/0.3*dAzimutal;
            RecordEnergyDeposition(step, hit, fraction);
            // right side
            if (phi > 0 && sector != 3){
                auto hitNeighbor = (*fHitsCollection)[cellID + 64];
                RecordEnergyDeposition(step, hitNeighbor, 1. - fraction);
            }
            // left side
            else if (phi < 0 && sector != 0){
                auto hitNeighbor = (*fHitsCollection)[cellID - 64];
                RecordEnergyDeposition(step, hitNeighbor, 1. - fraction);
            }
        }
    }
    else{
        RecordEnergyDeposition(step, hit, 1.0);
    }

    return true;
}

void LCSD::RecordEnergyDeposition(G4Step *step, LCHit* hit, double weight=1.0){
    G4double edep = step->GetTotalEnergyDeposit() * weight;
    G4double trackLen = step->GetStepLength() * weight;
    G4int trackID = step->GetTrack()->GetTrackID();
    G4ThreeVector pos = (step->GetPreStepPoint()->GetPosition() + step->GetPostStepPoint()->GetPosition())/2.;
    G4ThreeVector mom = (step->GetPreStepPoint()->GetMomentum() + step->GetPostStepPoint()->GetMomentum())/2.;
    G4double energy = (step->GetPreStepPoint()->GetKineticEnergy() + step->GetPostStepPoint()->GetKineticEnergy())/2.;
    G4int pdg = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

    hit->fTotalEdep += edep;
    hit->fTotalTrackLen += trackLen;
    if (trackID == 1) hit->fIsPrimary = 1;

    for(long unsigned int i=0; i < hit->fMcContId.size(); ++i){
        if(trackID == hit->fMcContId[i]){
            hit->fMcContEdep[i] += edep;
            hit->fMcContTrackLen[i] += trackLen;
            return;
        }
    }

    hit->fMcContId.push_back(trackID);
    hit->fMcContPos.push_back(pos);
    hit->fMcContMom.push_back(mom);
    hit->fMcContEnergy.push_back(energy);
    hit->fMcContEdep.push_back(edep);
    hit->fMcContTrackLen.push_back(trackLen);
    hit->fMcContPdg.push_back(pdg);
}
