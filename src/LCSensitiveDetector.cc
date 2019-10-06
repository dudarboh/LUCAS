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
    collectionName.insert("LCCalHC");
    collectionName.insert("LCTrHC");
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *HCE){
    fCalHC = new LCCalHitsCollection(SensitiveDetectorName, collectionName[0]);
    HCE->AddHitsCollection(GetCollectionID(0), fCalHC);

    fTrHC = new LCTrHitsCollection(SensitiveDetectorName, collectionName[1]);
    HCE->AddHitsCollection(GetCollectionID(1), fTrHC);

    // Create LCHit object for each pad position and add it to the hit collection.
    // Hit collection is created at the begining of each event
    for(G4int layer=2; layer<8; layer++){
        for(G4int sector=0; sector<4; sector++){
            for(G4int pad=0; pad<64; pad++){
                fCalHC->insert(new LCCalHit(sector, pad, layer));
            }
        }
    }
}

G4bool LCSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory*){
    // 1) Check the layer to see is it calorimeter or Tracker hit
    // 2) If calorimeter hit, just add the energy to the cell
    // 3) If tracker hit:
    // 3.1) Create LCTrHit if doesn't exist
    // 3.2) Add energy
    // 3.3) Add particle if enters the Tracker volome
    // Carbon fiber copy number
    auto prePoint = step->GetPreStepPoint();
    G4int layer = prePoint->GetTouchable()->GetCopyNumber(3);
    G4int pad = prePoint->GetTouchable()->GetReplicaNumber(0);
    G4int sector = prePoint->GetTouchable()->GetReplicaNumber(1);

    G4double e_dep = step->GetTotalEnergyDeposit();
    if(layer > 1 && e_dep > 0.){
        // Get the hit from the collection corresponding to this position
        G4int cellID = pad + 64 * sector + 256 * (layer-2);
        LCCalHit *hit = (*fCalHC)[cellID];

        //Add step energy to it
        hit->hit_energy += e_dep;
        return true;
    }

    if(layer <= 1){
        LCTrHit *hit = NULL;
        //Loop over collection and check whether this hit already exists
        for(G4int j=0; j<fTrHC->entries(); j++){
            LCTrHit* existed_hit = (*fTrHC)[j];

            if(existed_hit->hit_sector == sector && existed_hit->hit_pad == pad){
                hit = (*fTrHC)[j];
                break;
            }
        }
        if(!hit){
            hit = new LCTrHit(sector, pad, layer);
            fTrHC->insert(hit);
        }
        // Add energy
        hit->hit_energy += e_dep;

        //Add charged track length
        if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.){
            hit->hit_length += step->GetStepLength();
        }

        //If enters the volume add particles properties
        if(prePoint->GetStepStatus() == fGeomBoundary){
            hit->hit_n_particles ++;
            G4ThreeVector position = prePoint->GetPosition();
            hit->particle_x.push_back(position.getX());
            hit->particle_y.push_back(position.getY());
            hit->particle_z.push_back(position.getZ());
            
            G4ThreeVector momentum = prePoint->GetMomentum();
            hit->particle_px.push_back(momentum.getX());
            hit->particle_py.push_back(momentum.getY());
            hit->particle_pz.push_back(momentum.getZ());

            G4double kin_energy = prePoint->GetKineticEnergy();
            hit->particle_energy.push_back(kin_energy);

            G4int type = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();

            hit->particle_type.push_back(type);

            if(step->GetTrack()->GetTrackID() == 1){
                hit->particle_primary.push_back(1);
            }
            else{
                hit->particle_primary.push_back(0);
            }
        }
        return true;
    }
    return false;
}


