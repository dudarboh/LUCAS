#include "LCRun.hh"
#include "LCHit.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"

LCRun::LCRun(){
    SimulateNoise();
    G4RunManager::GetRunManager()->SetPrintProgress(1000);     

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
    long seeds[2];
    long systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());

    G4Random::setTheSeeds(seeds);
    analysisManager->OpenFile("output_" + std::to_string(seeds[0]) + "_" + std::to_string(seeds[1]) + ".root");

    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);
    analysisManager->CreateNtuple("LumiCal", "LumiCal TB simulation");

    analysisManager->CreateNtupleIColumn("hit_n");
    analysisManager->CreateNtupleIColumn("hit_sector", hit_sector);
    analysisManager->CreateNtupleIColumn("hit_pad", hit_pad);
    analysisManager->CreateNtupleIColumn("hit_layer", hit_layer);
    analysisManager->CreateNtupleDColumn("hit_energy", hit_energy);
    analysisManager->CreateNtupleIColumn("hit_primary", hit_primary);

    analysisManager->FinishNtuple();
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

        G4int hit_n = 0;
        LCHit *hit;
        //Simulation of efficiency of calorimeter
        G4double S0_cal = 0.819;
        G4double p1_cal = 2.166;
        G4double p0 = 0.999 / 2.;

        G4int sector, pad, layer;
        G4double noise_energy;
        G4double energy_in_mips;

        for(G4int i=0; i<HitsCollection->entries(); i++){
            hit = (*HitsCollection)[i];
            if(hit->GetEnergy() <= 0.) continue;

            sector = hit->GetSector();
            pad = hit->GetPad();
            layer = hit->GetLayer();

            noise_energy = G4RandGauss::shoot(0., 0.6*fApvNoise[sector][pad][layer]);
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
            hit_primary.push_back(hit->GetIsPrimary());
        
            hit_n ++;
        }
        analysisManager->FillNtupleIColumn(0, hit_n);
        
        analysisManager->AddNtupleRow();
        hit_sector.clear();
        hit_pad.clear();
        hit_layer.clear();
        hit_energy.clear();
        hit_primary.clear();
    }
    G4Run::RecordEvent(event);
}
