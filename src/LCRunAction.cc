#include "LCRunAction.hh"

#include "LCRun.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include <ctime>
#include <fstream>

std::chrono::time_point<std::chrono::system_clock> launchTime = std::chrono::system_clock::now();

LCRunAction::LCRunAction():G4UserRunAction(){;}

G4Run* LCRunAction::GenerateRun(){return (new LCRun);}

LCRunAction::~LCRunAction(){delete G4AnalysisManager::Instance();}

void LCRunAction::BeginOfRunAction(const G4Run* run){
    G4cout<<"Number of events to process: "<<run->GetNumberOfEventToBeProcessed()<<G4endl;
}

void LCRunAction::EndOfRunAction(const G4Run*){
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}

