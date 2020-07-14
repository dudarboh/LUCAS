#include "LCDetectorConstruction.hh"
#include "LCActionInitialization.hh"
#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4SystemOfUnits.hh"
#include "Randomize.hh"


int main(int argc, char** argv){
    std::cout<<"Start of main"<<std::endl;

    //Set random seeds
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    // If no arguments passed - visualization mode
    G4UIExecutive* ui = 0;
    if(argc == 1) ui = new G4UIExecutive(argc, argv);

    G4RunManager *runManager = new G4RunManager;
    G4cout<<"After RunManager is created"<<G4endl;

    runManager->SetUserInitialization(new LCDetectorConstruction);

    G4PhysListFactory factory;
    G4VModularPhysicsList *physicsList = factory.GetReferencePhysList("FTFP_BERT");
    physicsList->SetDefaultCutValue(0.001*mm);
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new LCActionInitialization);
    G4cout<<"After ActionInitialization is created"<<G4endl;

    // Visualization
    auto visManager = new G4VisExecutive;
    visManager->Initialise();

    auto UIManager = G4UImanager::GetUIpointer();

    if(!ui){
        // batch mode execute filname.mac
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UIManager->ApplyCommand(command+fileName);
    }
    else{
        // interactive mode
        // CHANGE THIS macroPath so it can find init_vis.mac
        UIManager->ApplyCommand("/control/macroPath ../");
        UIManager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    std::cout<<"End of main"<<std::endl;
    delete visManager;
    delete runManager;
}
