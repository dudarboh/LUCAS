#include "LCRunAction.hh"
#include "g4root.hh"

#include "G4Run.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "Randomize.hh"

#include <ctime>

std::chrono::time_point<std::chrono::system_clock> launchTime = std::chrono::system_clock::now();

LCRunAction::LCRunAction():G4UserRunAction(){
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

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
    analysisManager->CreateNtupleIColumn("hit_bs", hit_bs);

    analysisManager->FinishNtuple();
}

LCRunAction::~LCRunAction(){delete G4AnalysisManager::Instance();}


void LCRunAction::BeginOfRunAction(const G4Run* run){
    G4cout<<"Start of the run."<<G4endl;
    G4cout<<"Number of events to process: "<<run->GetNumberOfEventToBeProcessed()<<G4endl;

    long seeds[2];
    long systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());
    G4Random::setTheSeeds(seeds);

    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("Lucas_output_" + std::to_string(std::time(nullptr)) + ".root");
}

void LCRunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

void LCRunAction::FillEventData(const G4Event* event, LCHitsCollection *HitsCollection){
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

    G4int hit_n = HitsCollection->entries();
    analysisManager->FillNtupleIColumn(6, hit_n);

    LCHit *hit;

    for(G4int i=0; i<hit_n; i++){
        hit = (*HitsCollection)[i];
        hit_sector.push_back(hit->GetSector());
        hit_pad.push_back(hit->GetPad());
        hit_layer.push_back(hit->GetLayer());
        hit_energy.push_back(hit->GetEnergy());
        hit_bs.push_back(hit->GetBS());
    }
    analysisManager->AddNtupleRow();
}

void LCRunAction::ClearVectors(){
    hit_sector.clear();
    hit_pad.clear();
    hit_layer.clear();
    hit_energy.clear();
    hit_bs.clear();
}