#include "LCRunAction.hh"

#include <sys/times.h>
#include "Randomize.hh"

#include "G4Run.hh"
#include "G4RootAnalysisManager.hh"
#include "g4root.hh"

LCRunAction::LCRunAction(G4bool rootOutput){fRootOutput = rootOutput;}

LCRunAction::LCRunAction(){fRootOutput = 0;}

LCRunAction::~LCRunAction(){;}


void LCRunAction::BeginOfRunAction(const G4Run*){
    tms fTimeNow;
    clock_t StartTime = times(&fTimeNow);
    G4long seed = StartTime;
    G4RandGauss::setTheSeed(seed);

    if(fRootOutput){
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        analysisManager->OpenFile("Lucas_output.root");

        analysisManager->CreateNtuple("LumiCal", "LumiCal TB simulation");
        analysisManager->CreateNtupleDColumn("prim_x");
        analysisManager->CreateNtupleDColumn("prim_y");
        analysisManager->CreateNtupleDColumn("prim_z");

        analysisManager->CreateNtupleDColumn("prim_px");
        analysisManager->CreateNtupleDColumn("prim_py");
        analysisManager->CreateNtupleDColumn("prim_pz");

        analysisManager->CreateNtupleIColumn("hit_n");

        analysisManager->CreateNtupleDColumn("hit_x");
        analysisManager->CreateNtupleDColumn("hit_y");
        analysisManager->CreateNtupleDColumn("hit_z");

        analysisManager->CreateNtupleIColumn("hit_sector");
        analysisManager->CreateNtupleIColumn("hit_pad");
        analysisManager->CreateNtupleIColumn("hit_layer");

        analysisManager->CreateNtupleDColumn("hit_energy");
        analysisManager->CreateNtupleIColumn("hit_direction");
        analysisManager->CreateNtupleIColumn("hit_born_in_si");
        analysisManager->CreateNtupleIColumn("hit_pdg");
    }
}

void LCRunAction::EndOfRunAction(const G4Run*){
    if(fRootOutput){
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

        analysisManager->Write();
        analysisManager->CloseFile();
    }
}
