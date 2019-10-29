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
    auto particle_name = step->GetTrack()->GetParticleDefinition()->GetParticleName();
    auto track_id = step->GetTrack()->GetTrackID();


    G4double e_dep = step->GetTotalEnergyDeposit();

    // If calorimeter:
    if(layer > 1 && e_dep > 0.){
        // Get the hit from the collection corresponding to this position
        G4int cellID = pad + 64 * sector + 256 * (layer-2);
        LCCalHit *hit = (*fCalHC)[cellID];

        //Add step energy to it
        hit->energy += e_dep;
        return true;
    }

    // If tracker:
    if(layer <= 1){
        LCTrHit *hit = NULL;
        //Loop over collection and check whether this hit already exists
        for(G4int j=0; j<fTrHC->entries(); j++){
            LCTrHit* existed_hit = (*fTrHC)[j];

            if(existed_hit->sector == sector && existed_hit->pad == pad && existed_hit->layer == layer){
                hit = (*fTrHC)[j];
                break;
            }
        }
        if(!hit){
            hit = new LCTrHit(sector, pad, layer);
            fTrHC->insert(hit);
        }
        // Add energy and track length
        hit->energy += e_dep;
        G4double stepLength = 0.;
        if(step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) stepLength = step->GetStepLength();
        hit->track_len += stepLength;

        //If enters the volume add particles properties
        if(prePoint->GetStepStatus() == fGeomBoundary && hit->type != 0 && hit->type != 1){
            //Assign hit type:
            // 0 - mixed
            // 1 - primary
            // 2 - electron
            // 3 - gamma
            // 4 - positron
            // 5 - hadrons
            // If hit wasn't assigned yet, make an assignment
            if(hit->type == -1){
                //Make primary
                if (track_id == 1) hit->type = 1;
                // Make back-scattered
                else if (particle_name == "e-") hit->type = 2;
                else if (particle_name == "gamma") hit->type = 3;
                else if (particle_name == "e+") hit->type = 4;
                else if (particle_name == "pi-" || particle_name == "pi+" || particle_name == "neutron" || particle_name == "proton") hit->type = 5;
                else{
                    G4cout<<"THE UNASSIGNED PARTICLE:"<<particle_name<<G4endl;
                }
                G4ThreeVector position = prePoint->GetPosition();
                hit->particle_x = position.getX();
                hit->particle_y = position.getY();
                hit->particle_z = position.getZ();
                
                G4ThreeVector momentum = prePoint->GetMomentum();
                hit->particle_px = momentum.getX();
                hit->particle_py = momentum.getY();
                hit->particle_pz = momentum.getZ();

                hit->particle_energy = prePoint->GetKineticEnergy();
                hit->track_id = track_id;
            }
            // If was assignmed before, check whether it mixed or not
            else{
                if (particle_name == "e-" && hit->type != 2) hit->type = 0;
                else if (particle_name == "gamma" && hit->type != 3) hit->type = 0;
                else if (particle_name == "e+" && hit->type != 4) hit->type = 0;
                else if ((particle_name == "pi-"
                        || particle_name == "pi+"
                        || particle_name == "neutron"
                        || particle_name == "proton") && hit->type != 5) hit->type = 0;
            }
        } // end if boundary
        return true;
    } // end if tracker
    return false;
}


