#include "LCRunAction.hh"
#include "LCEventAction.hh"

#include "G4RunManager.hh"
#include "Randomize.hh"
#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "g4analysis.hh"

LCRunAction::LCRunAction(LCEventAction* eventAction, G4String OutputFileName):
G4UserRunAction(), fEventAction(eventAction){
    // std::cout<<"Start of LCRunAction::LCRunAction"<<std::endl;
    // Create the analysis manager using a new factory method.
    // The choice of analysis technology is done via the function argument.
    auto analysisManager = G4Analysis::ManagerInstance("root");
    // Default settings
    // Note: merging ntuples is available only with Root output
    analysisManager->SetNtupleMerging(true);
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetFileName(OutputFileName);

    // Creating ntuple
    if (fEventAction) {
      analysisManager->CreateNtuple("lumical", "mc");

      // Branches for trigger analysis
      analysisManager->CreateNtupleIColumn("n_triggers");  // column Id = 0
      analysisManager->CreateNtupleIColumn("trigger1");  // column Id = 1
      analysisManager->CreateNtupleIColumn("trigger2");  // column Id = 2
      analysisManager->CreateNtupleIColumn("trigger3");  // column Id = 3

      // Branches for calorimeter analysis
      analysisManager->CreateNtupleIColumn("cal_n_hits");  // column Id = 4
      analysisManager->CreateNtupleIColumn("cal_sector", fEventAction->fCalSector); //Id = 5
      analysisManager->CreateNtupleIColumn("cal_pad", fEventAction->fCalPad); // Id = 6
      analysisManager->CreateNtupleIColumn("cal_layer", fEventAction->fCalLayer); // Id = 7
      analysisManager->CreateNtupleDColumn("cal_energy", fEventAction->fCalEnergy); // Id = 8

      // Branches for Tracker1 analysis
      analysisManager->CreateNtupleIColumn("tr1_n_hits");  // column Id = 9
      analysisManager->CreateNtupleIColumn("tr1_sector", fEventAction->fTr1Sector); // Id = 10
      analysisManager->CreateNtupleIColumn("tr1_pad", fEventAction->fTr1Pad); // Id = 11
      analysisManager->CreateNtupleIColumn("tr1_layer", fEventAction->fTr1Layer); // Id = 12
      analysisManager->CreateNtupleDColumn("tr1_energy", fEventAction->fTr1Energy); // Id = 13
      analysisManager->CreateNtupleIColumn("tr1_type", fEventAction->fTr1Type); // Id = 14
      analysisManager->CreateNtupleDColumn("tr1_track_len", fEventAction->fTr1TrackLen); // Id = 15
      analysisManager->CreateNtupleDColumn("tr1_x", fEventAction->fTr1X); // Id = 16
      analysisManager->CreateNtupleDColumn("tr1_y", fEventAction->fTr1Y); // Id = 17
      analysisManager->CreateNtupleDColumn("tr1_z", fEventAction->fTr1Z); // Id = 18
      analysisManager->CreateNtupleDColumn("tr1_px", fEventAction->fTr1Px); // Id = 19
      analysisManager->CreateNtupleDColumn("tr1_py", fEventAction->fTr1Py); // Id = 20
      analysisManager->CreateNtupleDColumn("tr1_pz", fEventAction->fTr1Pz); // Id = 21
      analysisManager->CreateNtupleDColumn("tr1_p_energy", fEventAction->fTr1PEnergy); // Id = 22

      analysisManager->CreateNtupleIColumn("tr2_n_hits");  // column Id = 23
      analysisManager->CreateNtupleIColumn("tr2_sector", fEventAction->fTr2Sector); // Id = 24
      analysisManager->CreateNtupleIColumn("tr2_pad", fEventAction->fTr2Pad); // Id = 25
      analysisManager->CreateNtupleIColumn("tr2_layer", fEventAction->fTr2Layer); // Id = 26
      analysisManager->CreateNtupleDColumn("tr2_energy", fEventAction->fTr2Energy); // Id = 27
      analysisManager->CreateNtupleIColumn("tr2_type", fEventAction->fTr2Type); // Id = 28
      analysisManager->CreateNtupleDColumn("tr2_track_len", fEventAction->fTr2TrackLen); // Id = 29
      analysisManager->CreateNtupleDColumn("tr2_x", fEventAction->fTr2X); // Id = 30
      analysisManager->CreateNtupleDColumn("tr2_y", fEventAction->fTr2Y); // Id = 31
      analysisManager->CreateNtupleDColumn("tr2_z", fEventAction->fTr2Z); // Id = 32
      analysisManager->CreateNtupleDColumn("tr2_px", fEventAction->fTr2Px); // Id = 33
      analysisManager->CreateNtupleDColumn("tr2_py", fEventAction->fTr2Py); // Id = 34
      analysisManager->CreateNtupleDColumn("tr2_pz", fEventAction->fTr2Pz); // Id = 35
      analysisManager->CreateNtupleDColumn("tr2_p_energy", fEventAction->fTr2PEnergy); // Id = 36

      analysisManager->FinishNtuple();
    }
    // std::cout<<"End of LCRunAction::LCRunAction"<<std::endl;
}

LCRunAction::~LCRunAction(){
    delete G4AnalysisManager::Instance();
}

void LCRunAction::BeginOfRunAction(const G4Run* ){
    // std::cout<<"Start of LCRunAction::BeginOfRunAction"<<std::endl;
    #ifdef RUN_PH
    std::cout<<"Start of Photon Run"<<std::endl;
    #else
    std::cout<<"Start of Electron Run"<<std::endl;
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
    analysisManager->OpenFile();

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
