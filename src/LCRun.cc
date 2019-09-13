#include "LCRun.hh"
#include "LCHit.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"

LCRun::LCRun(){
    SimulateNoise();
    G4RunManager::GetRunManager()->SetPrintProgress(1000);     

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);
    analysisManager->CreateNtuple("LumiCal", "LumiCal TB simulation");
    analysisManager->CreateNtupleDColumn("prim_x");
    analysisManager->CreateNtupleDColumn("prim_y");
    analysisManager->CreateNtupleDColumn("prim_z");
    analysisManager->CreateNtupleDColumn("prim_px");
    analysisManager->CreateNtupleDColumn("prim_py");
    analysisManager->CreateNtupleDColumn("prim_pz");

    analysisManager->CreateNtupleIColumn("hit_n");
    analysisManager->CreateNtupleIColumn("hit_sector", hit_sector);
    analysisManager->CreateNtupleIColumn("hit_pad", hit_pad);
    analysisManager->CreateNtupleIColumn("hit_layer", hit_layer);
    analysisManager->CreateNtupleDColumn("hit_energy", hit_energy);
    analysisManager->CreateNtupleIColumn("n_bs_particles", n_bs_particles);
    analysisManager->CreateNtupleIColumn("n_dir_particles", n_dir_particles);

    analysisManager->FinishNtuple();

    long seeds[2];
    long systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());

    G4Random::setTheSeeds(seeds);
    analysisManager->OpenFile("output_" + std::to_string(seeds[0]) + "_" + std::to_string(seeds[1]) + ".root");
}

LCRun::~LCRun(){;}


void LCRun::SimulateNoise(){
    std::ifstream file;
    file.open("../noise.txt");

    if(!file.is_open()){
        G4ExceptionDescription msg;
        msg<<"Cannot find noise file";
        G4Exception("LCRunAction::SimulateNoise()", "no file found to simulate noise", FatalException, msg);
    }
    std::string line;
    std::getline(file, line); // Skip 1st line

    G4int sector;
    G4int pad;
    G4int layer;
    G4double noise;

    while(file>>sector>>pad>>layer>>noise){
        if(sector != -1) fApvNoise[sector][pad][layer] = noise;
    }
    file.close();
}

void LCRun::RecordEvent(const G4Event* event){
    G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID("LumiCalHitsCollection"); 

    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    if(HCE){
        auto HitsCollection = static_cast<LCHitsCollection*>(HCE->GetHC(HCID));

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
        G4int hit_n = 0;

        LCHit *hit;

        //Simulation of efficiency of calorimeter
        // Paper numbers
        G4double S0_cal = 0.819;
        G4double p1_cal = 2.166;
        G4double p0 = 0.999 / 2.;

        G4double noise_energy;
        G4double energy_in_mips;

        for(G4int i=0; i<HitsCollection->entries(); i++){
            hit = (*HitsCollection)[i];
            if(hit->GetEnergy() <= 0.) continue;

            G4int sector = hit->GetSector();
            G4int pad = hit->GetPad();
            G4int layer = hit->GetLayer();

            noise_energy = G4RandGauss::shoot(0., 0.1*fApvNoise[sector][pad][layer]);
            hit->AddHitEnergy(noise_energy);

            energy_in_mips = (hit->GetEnergy())/0.0885;
            if(energy_in_mips <= 0. || sector == 0 || sector == 3 || pad < 20) continue;

            if(layer > 1){
                if(G4UniformRand() > (1. + std::erf((energy_in_mips - S0_cal) / p1_cal)) * p0) continue;
            }
             
            hit_sector.push_back(sector);
            hit_pad.push_back(pad);
            hit_layer.push_back(layer);
            hit_energy.push_back(energy_in_mips); // Write energy in MIPs not MeV
            n_bs_particles.push_back(hit->GetBS());
            n_dir_particles.push_back(hit->GetDir());
        
            hit_n ++;
        }
        analysisManager->FillNtupleIColumn(6, hit_n);
        
        analysisManager->AddNtupleRow();
        hit_sector.clear();
        hit_pad.clear();
        hit_layer.clear();
        hit_energy.clear();
        n_bs_particles.clear();
        n_dir_particles.clear();
    }
    G4Run::RecordEvent(event);
}
