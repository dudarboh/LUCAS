#include "LCRunAction.hh"

#include "LCRun.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include <fstream>

LCRunAction::LCRunAction():G4UserRunAction(){;}

// Generate User Custom Run to write an output root file and analyse events
G4Run* LCRunAction::GenerateRun(){
    return (new LCRun);
}

LCRunAction::~LCRunAction(){
    delete G4AnalysisManager::Instance();
}

// Print number of events to process in the begining of a run
void LCRunAction::BeginOfRunAction(const G4Run* run){
    G4cout<<"Number of events to process: "<<run->GetNumberOfEventToBeProcessed()<<G4endl;
}

// Write and close output root file
void LCRunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

