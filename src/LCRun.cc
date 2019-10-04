#include "LCRun.hh"
#include "LCHit.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"

LCRun::LCRun(){
    // Measure launch time
    fLaunchTime = std::chrono::system_clock::now();

    // This puts APV channel noise values into fApvNoise array
    SimulateNoise();

    // Print every 1000th event
    G4RunManager::GetRunManager()->SetPrintProgress(1000);     

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
    // Make new random seeds
    long seeds[2];
    long systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());
    G4Random::setTheSeeds(seeds);

    // Create new output root file
    analysisManager->OpenFile("output_" + std::to_string(seeds[0]) + "_" + std::to_string(seeds[1]) + ".root");

    analysisManager->SetVerboseLevel(1);
    //Merge in the end if you run on multiple threads
    analysisManager->SetNtupleMerging(true);
    //Create a tree
    analysisManager->CreateNtuple("LumiCal", "LumiCal TB simulation");

    //Columns for the tree to be written
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
    //Find ../noise.txt file
    std::ifstream file;
    file.open("../noise.txt");

    //This exception will not help to find the error :c
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

    //Write values into array
    while(file>>sector>>pad>>layer>>noise){
        if(sector != -1) fApvNoise[sector][pad][layer] = noise;
    }
    file.close();
}

void LCRun::RecordEvent(const G4Event* event){
    //Count how much time elapsed
    if(event->GetEventID() % 1000 == 0){
        std::chrono::duration<double> timeElapsed = std::chrono::system_clock::now() - fLaunchTime;
        G4cout<<"Time elapsed: "<<timeElapsed.count()<<" sec"<<G4endl;
    }

    //Get hits collection ID for this event
    G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID("LumiCalHitsCollection"); 

    G4HCofThisEvent* HCE = event->GetHCofThisEvent();

    if(HCE){
        //Get hits collection for this event
        auto HitsCollection = static_cast<LCHitsCollection*>(HCE->GetHC(HCID));

        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        G4int hit_n = 0;
        LCHit *hit;
        
        //Simulation of efficiency of the calorimeter
        G4double S0_cal = 0.819;
        G4double p1_cal = 2.166;
        G4double p0 = 0.999 / 2.;

        G4int sector, pad, layer;
        G4double noise_energy;
        G4double energy_in_mips;

        // Loop over all hits
        for(G4int i=0; i<HitsCollection->entries(); i++){
            hit = (*HitsCollection)[i];

            if(hit->GetEnergy() <= 0.) continue;

            //Extract hits coordinates
            sector = hit->GetSector();
            pad = hit->GetPad();
            layer = hit->GetLayer();

            //Calculate electronic noise as a Gauss with mean - 0, sigma - 0.6*apvNoise
            noise_energy = G4RandGauss::shoot(0., 0.6*fApvNoise[sector][pad][layer]);
            //Add this to the hit energy
            hit->AddHitEnergy(noise_energy);

            // Convert energy from MeV into MIPs
            energy_in_mips = (hit->GetEnergy())/0.0885;

            //This is COMPLETELY MY ANALYSIS CUT region to lower a computation time a bit
            // if(energy_in_mips <= 0. || sector == 0 || sector == 3 || pad < 20) continue;

            // If hit is in calorimeter - reject hit with a probability based on efficiency curve
            // measured for the paper
            if(layer > 1){
                if(G4UniformRand() > (1. + std::erf((energy_in_mips - S0_cal) / p1_cal)) * p0) continue;
            }
             
            // Write hit in the root file
            hit_sector.push_back(sector);
            hit_pad.push_back(pad);
            hit_layer.push_back(layer);
            hit_energy.push_back(energy_in_mips); // Write energy in MIPs not MeV
            hit_primary.push_back(hit->GetIsPrimary());
        
            hit_n ++;
        }
        // Write number of hits
        analysisManager->FillNtupleIColumn(0, hit_n);
        
        analysisManager->AddNtupleRow();

        // Clear all vectors
        hit_sector.clear();
        hit_pad.clear();
        hit_layer.clear();
        hit_energy.clear();
        hit_primary.clear();
    }
    //Idk why I need it, but its in the example and works fine
    G4Run::RecordEvent(event);
}
