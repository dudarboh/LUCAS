#include "LCRunAction.hh"
#include "LCEventAction.hh"

#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "LCAnalysis.hh"

LCRunAction::LCRunAction(LCEventAction* eventAction, G4String OutputFileName):
G4UserRunAction(), fEventAction(eventAction), fOutputFileName(OutputFileName){
    // std::cout<<"Start of LCRunAction::LCRunAction"<<std::endl;
    G4RunManager::GetRunManager()->SetPrintProgress(1);

    // Create the analysis manager using a new factory method.
    // The choice of analysis technology is done via the function argument.
    auto analysisManager = G4AnalysisManager::Instance();
    // Default settings
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);

    // Creating ntuple
    analysisManager->CreateNtuple("lumical", "mc");

    // Branches for trigger analysis
    analysisManager->CreateNtupleIColumn("n_triggers");  // column Id = 0
    analysisManager->CreateNtupleIColumn("trigger1");  // column Id = 1
    analysisManager->CreateNtupleIColumn("trigger2");  // column Id = 2
    analysisManager->CreateNtupleIColumn("trigger3");  // column Id = 3

    // Branches for calorimeter analysis
    analysisManager->CreateNtupleIColumn("n_hits");  // column Id = 4
    analysisManager->CreateNtupleIColumn("sector", fEventAction->fSector); //Id = 5
    analysisManager->CreateNtupleIColumn("pad", fEventAction->fPad); // Id = 6
    analysisManager->CreateNtupleIColumn("layer", fEventAction->fLayer); // Id = 7
    analysisManager->CreateNtupleIColumn("primary", fEventAction->fIsPrimary); // Id = 7
    analysisManager->CreateNtupleDColumn("total_edep", fEventAction->fTotalEdep); // Id = 8
    analysisManager->CreateNtupleDColumn("total_track_len", fEventAction->fTotalTrackLen); // Id = 8
    analysisManager->CreateNtupleIColumn("mc_cont_id", fEventAction->fMcContId); // Id = 9
    analysisManager->CreateNtupleDColumn("mc_cont_posx", fEventAction->fMcContPosX); // Id = 10
    analysisManager->CreateNtupleDColumn("mc_cont_posy", fEventAction->fMcContPosY); // Id = 11
    analysisManager->CreateNtupleDColumn("mc_cont_posz", fEventAction->fMcContPosZ); // Id = 12
    analysisManager->CreateNtupleDColumn("mc_cont_momx", fEventAction->fMcContMomX); // Id = 13
    analysisManager->CreateNtupleDColumn("mc_cont_momy", fEventAction->fMcContMomY); // Id = 14
    analysisManager->CreateNtupleDColumn("mc_cont_momz", fEventAction->fMcContMomZ); // Id = 15
    analysisManager->CreateNtupleDColumn("mc_cont_energy", fEventAction->fMcContEnergy); // Id = 16
    analysisManager->CreateNtupleDColumn("mc_cont_edep", fEventAction->fMcContEdep); // Id = 17
    analysisManager->CreateNtupleDColumn("mc_cont_track_len", fEventAction->fMcContTrackLen); // Id = 18
    analysisManager->CreateNtupleIColumn("mc_cont_pdg", fEventAction->fMcContPdg); // Id = 19

    analysisManager->FinishNtuple();
    // std::cout<<"End of LCRunAction::LCRunAction"<<std::endl;
}

LCRunAction::~LCRunAction(){
    delete G4AnalysisManager::Instance();
}

void LCRunAction::BeginOfRunAction(const G4Run* ){
    // std::cout<<"Start of LCRunAction::BeginOfRunAction"<<std::endl;
    #ifdef RUN_PH
    // std::cout<<"Start of Photon Run"<<std::endl;
    #else
    // std::cout<<"Start of Electron Run"<<std::endl;
    #endif
    // automatic (time-based) random seeds for each run
    long seeds[2];
    time_t systime = time(NULL);
    seeds[0] = (long) systime;
    seeds[1] = (long) (systime*G4UniformRand());
    G4Random::setTheSeeds(seeds);
    G4Random::showEngineStatus();

    //inform the runManager to save random number seed
    // G4RunManager::GetRunManager()->SetRandomNumberStore(true);

    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    // Open an output file
    // The default file name is set in LCRunAction::LCRunAction(),
    // it can be overwritten in a macro
    analysisManager->OpenFile(fOutputFileName);

    // std::cout<<"End of LCRunAction::BeginOfRunAction"<<std::endl;
}
// Write and close output root file
void LCRunAction::EndOfRunAction(const G4Run*){
    // std::cout<<"Start of LCRunAction::EndOfRunAction"<<std::endl;
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
    // std::cout<<"End of LCRunAction::EndOfRunAction"<<std::endl;
}
