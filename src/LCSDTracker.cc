#include "LCSDTracker.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSDTracker::LCSDTracker(G4String detector_name) :G4VSensitiveDetector(detector_name),
fTrackerHCID(-1){
    collectionName.insert("LCTrackerHC");
}

LCSDTracker::~LCSDTracker(){}

void LCSDTracker::Initialize(G4HCofThisEvent *HCE){
    // std::cout<<"Start of LCSDTracker::Initialize"<<std::endl;

    fTrackerHC = new LCTrackerHC(SensitiveDetectorName, collectionName[0]);
    if (fTrackerHCID<0) fTrackerHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fTrackerHC);
    HCE->AddHitsCollection(fTrackerHCID, fTrackerHC);
    // std::cout<<"End of LCSDTracker::Initialize"<<std::endl;
}

G4bool LCSDTracker::ProcessHits(G4Step *step, G4TouchableHistory*){
    // std::cout<<"Start of LCSDTracker::ProcessHits"<<std::endl;

    // If hit wasn't assigned yet
    // or it has gamma writen instead of electron
    // or electron is with lower energy is written instead of the bigger one
    // reasign particle properties.

    G4double e_dep = step->GetTotalEnergyDeposit();
    if (e_dep <= 0.) return true;

    auto prePoint = step->GetPreStepPoint();
    auto touchable = prePoint->GetTouchable();
    auto track = step->GetTrack();

    G4int sector = touchable->GetReplicaNumber(1);
    G4int pad = touchable->GetReplicaNumber(0);
    G4int layer = touchable->GetCopyNumber(3);

    auto particle_name = track->GetParticleDefinition()->GetParticleName();
    auto track_id = track->GetTrackID();

    LCHitTracker *hit = NULL;

    //If hit exists
    for(unsigned int j=0; j<fTrackerHC->entries(); j++){
        LCHitTracker* existed_hit = (*fTrackerHC)[j];
        if(existed_hit->sector == sector && existed_hit->pad == pad && existed_hit->layer == layer){
            hit = existed_hit;
            break;
        }
    }
    // If hit doesn't exist
    if(!hit){
        hit = new LCHitTracker(sector, pad, layer);
        fTrackerHC->insert(hit);
    }

    // Add energy and track length
    hit->energy += e_dep;
    auto charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
    if(charge != 0.) hit->track_len += step->GetStepLength();

    auto type_unassigned = hit->type == -1;
    auto type_neutral = hit->type > 9;

    //If enters the volume and not primary check particles properties
    if(prePoint->GetStepStatus() == fGeomBoundary && hit->type != 1){
        if(type_unassigned || (type_neutral && charge != 0.)
        || (!type_neutral && (hit->p_energy < prePoint->GetKineticEnergy()))){
            //Change its type and pos+mom
            //Make primary
            if(track_id == 1) hit->type = 1;
            // Make 2nd
            else if (particle_name == "e-") hit->type = 2;
            else if (particle_name == "e+") hit->type = 3;
            else if (particle_name == "mu-") hit->type = 4;
            else if (particle_name == "mu+") hit->type = 5;
            else if (particle_name == "proton") hit->type = 6;
            else if (particle_name == "deuteron") hit->type = 7;
            else if (particle_name == "pi-") hit->type = 8;
            else if (particle_name == "pi+") hit->type = 9;
            else if (particle_name == "gamma") hit->type = 10;
            else if (particle_name == "neutron") hit->type = 11;
            else if (particle_name == "nu_e") hit->type = 12;
            else if (particle_name == "anti_nu_e") hit->type = 13;
            else if (particle_name == "nu_mu") hit->type = 14;
            else if (particle_name == "anti_nu_mu") hit->type = 15;
            else G4cout<<"THE UNASSIGNED PARTICLE: "<<particle_name<<G4endl;

            G4ThreeVector position = prePoint->GetPosition();
            hit->x = position.getX();
            hit->y = position.getY();
            hit->z = position.getZ();

            G4ThreeVector momentum = prePoint->GetMomentum();
            hit->px = momentum.getX();
            hit->py = momentum.getY();
            hit->pz = momentum.getZ();
            hit->p_energy = prePoint->GetKineticEnergy();
        }
    }
    // std::cout<<"End of LCSDTracker::ProcessHits"<<std::endl;
    return true;
}
