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
    G4RunManager::GetRunManager()->SetPrintProgress(200);

    // Create the analysis manager using a new factory method.
    // The choice of analysis technology is done via the function argument.
    auto analysisManager = G4AnalysisManager::Instance();
    // Default settings
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);

    // Creating ntuple
    analysisManager->CreateNtuple("lumical", "mc");

    // Branches for trigger analysis
    analysisManager->CreateNtupleIColumn("n_triggers");
    analysisManager->CreateNtupleIColumn("trigger1");
    analysisManager->CreateNtupleIColumn("trigger2");
    analysisManager->CreateNtupleIColumn("trigger3");

    analysisManager->CreateNtupleIColumn("n_hits");
    analysisManager->CreateNtupleIColumn("sector", fEventAction->fSector);
    analysisManager->CreateNtupleIColumn("pad", fEventAction->fPad);
    analysisManager->CreateNtupleIColumn("layer", fEventAction->fLayer);
    analysisManager->CreateNtupleDColumn("energy", fEventAction->fTotalEdep);
    analysisManager->CreateNtupleIColumn("primary", fEventAction->fIsPrimary);
    analysisManager->CreateNtupleDColumn("track_len", fEventAction->fTotalTrackLen);
    analysisManager->CreateNtupleIColumn("n_mc_cont", fEventAction->fNMcCont);
    analysisManager->CreateNtupleIColumn("mc_cont_id", fEventAction->fMcContId);
    analysisManager->CreateNtupleDColumn("mc_cont_posx", fEventAction->fMcContPosX);
    analysisManager->CreateNtupleDColumn("mc_cont_posy", fEventAction->fMcContPosY);
    analysisManager->CreateNtupleDColumn("mc_cont_posz", fEventAction->fMcContPosZ);
    analysisManager->CreateNtupleDColumn("mc_cont_momx", fEventAction->fMcContMomX);
    analysisManager->CreateNtupleDColumn("mc_cont_momy", fEventAction->fMcContMomY);
    analysisManager->CreateNtupleDColumn("mc_cont_momz", fEventAction->fMcContMomZ);
    analysisManager->CreateNtupleDColumn("mc_cont_energy", fEventAction->fMcContEnergy);
    analysisManager->CreateNtupleDColumn("mc_cont_edep", fEventAction->fMcContEdep);
    analysisManager->CreateNtupleDColumn("mc_cont_track_len", fEventAction->fMcContTrackLen);
    analysisManager->CreateNtupleIColumn("mc_cont_pdg", fEventAction->fMcContPdg);

    analysisManager->FinishNtuple();
    // std::cout<<"End of LCRunAction::LCRunAction"<<std::endl;
}

LCRunAction::~LCRunAction(){
    delete G4AnalysisManager::Instance();
}

void LCRunAction::BeginOfRunAction(const G4Run* ){
    // std::cout<<"Start of LCRunAction::BeginOfRunAction"<<std::endl;

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
