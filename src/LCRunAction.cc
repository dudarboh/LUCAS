#include "LCRunAction.hh"

#include "g4root.hh"
#include "G4Run.hh"

std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

LCRunAction::LCRunAction():G4UserRunAction(){fEventData = 0;}


LCRunAction::LCRunAction(LCEventData *EventData):G4UserRunAction(){

    fEventData = EventData;
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    analysisManager->CreateNtuple("LumiCal", "LumiCal TB simulation");
    analysisManager->CreateNtupleDColumn("prim_x");
    analysisManager->CreateNtupleDColumn("prim_y");
    analysisManager->CreateNtupleDColumn("prim_z");

    analysisManager->CreateNtupleDColumn("prim_px");
    analysisManager->CreateNtupleDColumn("prim_py");
    analysisManager->CreateNtupleDColumn("prim_pz");

    analysisManager->CreateNtupleIColumn("hit_n");

    analysisManager->CreateNtupleDColumn("hit_x", EventData->hit_x);
    analysisManager->CreateNtupleDColumn("hit_y", EventData->hit_y);
    analysisManager->CreateNtupleDColumn("hit_z", EventData->hit_z);

    analysisManager->CreateNtupleIColumn("hit_sector", EventData->hit_sector);
    analysisManager->CreateNtupleIColumn("hit_pad", EventData->hit_pad);
    analysisManager->CreateNtupleIColumn("hit_layer", EventData->hit_layer);

    analysisManager->CreateNtupleDColumn("hit_energy", EventData->hit_energy);
    analysisManager->CreateNtupleIColumn("hit_direction", EventData->hit_direction);
    analysisManager->CreateNtupleIColumn("hit_pdg", EventData->hit_pdg);

    analysisManager->FinishNtuple();
}

LCRunAction::~LCRunAction(){;}


void LCRunAction::BeginOfRunAction(const G4Run* run){
    G4cout<<"Number of events: "<<run->GetNumberOfEventToBeProcessed()<<G4endl;

    if(fEventData){
        G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
        analysisManager->OpenFile("Lucas_output.root");
    }
}

void LCRunAction::EndOfRunAction(const G4Run*){
    if(fEventData){
        G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
        analysisManager->Write();
        analysisManager->CloseFile();
    }
}
