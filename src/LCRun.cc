#include "LCRun.hh"
#include "LCTrHit.hh"
#include "LCCalHit.hh"

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
    analysisManager->CreateNtuple("mc", "LumiCal TB16 simulation");

    //Columns for the tree to be written
    analysisManager->CreateNtupleIColumn("tr1_n_hits");
    analysisManager->CreateNtupleIColumn("tr2_n_hits");
    analysisManager->CreateNtupleIColumn("cal_n_hits");
    analysisManager->CreateNtupleIColumn("tr1_hit_sector", tr1_hit_sector);
    analysisManager->CreateNtupleIColumn("tr1_hit_pad", tr1_hit_pad);
    analysisManager->CreateNtupleIColumn("tr1_hit_layer", tr1_hit_layer);
    analysisManager->CreateNtupleDColumn("tr1_hit_energy", tr1_hit_energy);
    analysisManager->CreateNtupleIColumn("tr1_hit_type", tr1_hit_type);
    analysisManager->CreateNtupleDColumn("tr1_hit_track_len", tr1_hit_track_len);

    analysisManager->CreateNtupleDColumn("tr1_particle_x", tr1_particle_x);
    analysisManager->CreateNtupleDColumn("tr1_particle_y", tr1_particle_y);
    analysisManager->CreateNtupleDColumn("tr1_particle_z", tr1_particle_z);
    analysisManager->CreateNtupleDColumn("tr1_particle_px", tr1_particle_px);
    analysisManager->CreateNtupleDColumn("tr1_particle_py", tr1_particle_py);
    analysisManager->CreateNtupleDColumn("tr1_particle_pz", tr1_particle_pz);
    analysisManager->CreateNtupleDColumn("tr1_particle_energy", tr1_particle_energy);

    analysisManager->CreateNtupleIColumn("tr2_hit_sector", tr2_hit_sector);
    analysisManager->CreateNtupleIColumn("tr2_hit_pad", tr2_hit_pad);
    analysisManager->CreateNtupleIColumn("tr2_hit_layer", tr2_hit_layer);
    analysisManager->CreateNtupleDColumn("tr2_hit_energy", tr2_hit_energy);
    analysisManager->CreateNtupleIColumn("tr2_hit_type", tr2_hit_type);
    analysisManager->CreateNtupleDColumn("tr2_hit_track_len", tr2_hit_track_len);

    analysisManager->CreateNtupleDColumn("tr2_particle_x", tr2_particle_x);
    analysisManager->CreateNtupleDColumn("tr2_particle_y", tr2_particle_y);
    analysisManager->CreateNtupleDColumn("tr2_particle_z", tr2_particle_z);
    analysisManager->CreateNtupleDColumn("tr2_particle_px", tr2_particle_px);
    analysisManager->CreateNtupleDColumn("tr2_particle_py", tr2_particle_py);
    analysisManager->CreateNtupleDColumn("tr2_particle_pz", tr2_particle_pz);
    analysisManager->CreateNtupleDColumn("tr2_particle_energy", tr2_particle_energy);

    analysisManager->CreateNtupleIColumn("cal_hit_sector", cal_hit_sector);
    analysisManager->CreateNtupleIColumn("cal_hit_pad", cal_hit_pad);
    analysisManager->CreateNtupleIColumn("cal_hit_layer", cal_hit_layer);
    analysisManager->CreateNtupleDColumn("cal_hit_energy", cal_hit_energy);

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

    //Get hits collections IDs
    G4int CalHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LCCalHC"); 
    G4int TrHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LCTrHC"); 

    G4HCofThisEvent* HCE = event->GetHCofThisEvent();

    if(HCE){
        //Get cal and tracker hits collection for this event
        auto CalHC = static_cast<LCCalHitsCollection*>(HCE->GetHC(CalHCID));
        auto TrHC = static_cast<LCTrHitsCollection*>(HCE->GetHC(TrHCID));

        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        G4int cal_n_hits = 0;
        G4int tr1_n_hits = 0;
        G4int tr2_n_hits = 0;
        LCCalHit *cal_hit;
        LCTrHit *tr_hit;
        
        //Simulation of efficiency of the calorimeter
        G4double S0_cal = 0.819;
        G4double p1_cal = 2.166;
        G4double p0 = 0.999 / 2.;

        G4int sector, pad, layer;
        G4double noise_energy;
        G4double energy_in_mips;

        // Loop over calorimeter hits
        for(G4int i=0; i<CalHC->entries(); i++){
            cal_hit = (*CalHC)[i];

            if(cal_hit->energy <= 0.) continue;

            //Extract hits coordinates
            sector = cal_hit->sector;
            pad = cal_hit->pad;
            layer = cal_hit->layer;

            //Calculate electronic noise as a Gauss with mean - 0, sigma - 0.6*apvNoise
            noise_energy = G4RandGauss::shoot(0., 0.6*fApvNoise[sector][pad][layer]);
            //Add this to the hit energy
            cal_hit->energy += noise_energy;

            // Convert energy from MeV into MIPs
            energy_in_mips = (cal_hit->energy)/0.0885;

            // If hit is in calorimeter - reject hit with a probability based on efficiency curve
            // measured for the paper
            if(G4UniformRand() > (1. + std::erf((energy_in_mips - S0_cal) / p1_cal)) * p0) continue;
             
            // Write hit in the root file
            cal_hit_sector.push_back(sector);
            cal_hit_pad.push_back(pad);
            cal_hit_layer.push_back(layer);
            cal_hit_energy.push_back(energy_in_mips); // Write energy in MIPs not MeV
        
            cal_n_hits ++;
        }
        // Write number of hits
        analysisManager->FillNtupleIColumn(2, cal_n_hits);
        
        for(G4int i=0; i<TrHC->entries(); i++){
            tr_hit = (*TrHC)[i];
            if(tr_hit->energy <= 0.) continue;


            //Extract hits coordinates
            sector = tr_hit->sector;
            pad = tr_hit->pad;
            layer = tr_hit->layer;

            //Calculate electronic noise as a Gauss with mean - 0, sigma - 0.6*apvNoise
            noise_energy = G4RandGauss::shoot(0., 0.6*fApvNoise[sector][pad][layer]);
            //Add this to the hit energy
            tr_hit->energy += noise_energy;

            // Convert energy from MeV into MIPs
            energy_in_mips = (tr_hit->energy)/0.0885;
             
            // Write tr1 hit in the root file
            if(layer == 0){
                tr1_hit_sector.push_back(sector);
                tr1_hit_pad.push_back(pad);
                tr1_hit_layer.push_back(layer);
                tr1_hit_energy.push_back(energy_in_mips); // Write energy in MIPs not MeV
                tr1_hit_type.push_back(tr_hit->type);
                tr1_hit_track_len.push_back(tr_hit->track_len);

                tr1_particle_x.push_back(tr_hit->particle_x);
                tr1_particle_y.push_back(tr_hit->particle_y);
                tr1_particle_z.push_back(tr_hit->particle_z);
                tr1_particle_px.push_back(tr_hit->particle_px);
                tr1_particle_py.push_back(tr_hit->particle_py);
                tr1_particle_pz.push_back(tr_hit->particle_pz);
                tr1_particle_energy.push_back(tr_hit->particle_energy);
                tr1_n_hits ++;

            }
            // Write tr2 hit in the root file
            if(layer == 1){
                tr2_hit_sector.push_back(sector);
                tr2_hit_pad.push_back(pad);
                tr2_hit_layer.push_back(layer);
                tr2_hit_energy.push_back(energy_in_mips); // Write energy in MIPs not MeV
                tr2_hit_type.push_back(tr_hit->type);
                tr2_hit_track_len.push_back(tr_hit->track_len);

                tr2_particle_x.push_back(tr_hit->particle_x);
                tr2_particle_y.push_back(tr_hit->particle_y);
                tr2_particle_z.push_back(tr_hit->particle_z);
                tr2_particle_px.push_back(tr_hit->particle_px);
                tr2_particle_py.push_back(tr_hit->particle_py);
                tr2_particle_pz.push_back(tr_hit->particle_pz);
                tr2_particle_energy.push_back(tr_hit->particle_energy);
                tr2_n_hits ++;
            }
        }
        // Write number of hits
        analysisManager->FillNtupleIColumn(0, tr1_n_hits);
        analysisManager->FillNtupleIColumn(1, tr2_n_hits);

        analysisManager->AddNtupleRow();

        // Clear all vectors
        tr1_hit_sector.clear();
        tr1_hit_pad.clear();
        tr1_hit_layer.clear();
        tr1_hit_energy.clear();
        tr1_hit_type.clear();
        tr1_hit_track_len.clear();

        tr1_particle_x.clear();
        tr1_particle_y.clear();
        tr1_particle_z.clear();
        tr1_particle_px.clear();
        tr1_particle_py.clear();
        tr1_particle_pz.clear();
        tr1_particle_energy.clear();

        tr2_hit_sector.clear();
        tr2_hit_pad.clear();
        tr2_hit_layer.clear();
        tr2_hit_energy.clear();
        tr2_hit_type.clear();
        tr2_hit_track_len.clear();

        tr2_particle_x.clear();
        tr2_particle_y.clear();
        tr2_particle_z.clear();
        tr2_particle_px.clear();
        tr2_particle_py.clear();
        tr2_particle_pz.clear();
        tr2_particle_energy.clear();

        cal_hit_sector.clear();
        cal_hit_pad.clear();
        cal_hit_layer.clear();
        cal_hit_energy.clear();
    }
    //Idk why I need it, but its in the example and works fine
    G4Run::RecordEvent(event);
}
