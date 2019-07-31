// Include ROOT first to avoid shadowing 's' declaration warning due to ROOT/Geant4 names conflict

#include "LCDetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "QGSP_BERT.hh"
#include "LCRunAction.hh"
#include "LCEventAction.hh"
#include "LCHit.hh"

#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <G4HadronicProcessStore.hh>
#include <G4ProductionCutsTable.hh>

int main(int argc, char** argv){

    G4UIExecutive* ui = 0;
    if(argc == 1) ui = new G4UIExecutive(argc, argv);

    G4RunManager *runManager = new G4RunManager;

    // Detectron construction and physics list
    runManager->SetUserInitialization(new LCDetectorConstruction);

    G4VUserPhysicsList *physicsList = new QGSP_BERT(0);
    physicsList->SetDefaultCutValue(0.005*mm);

    runManager->SetUserInitialization(physicsList);
 
    // User Action Classes
    runManager->SetUserAction(new PrimaryGeneratorAction);

//    LCRootOut *theRootOut = 0;
    LCRunAction *theRunAction = 0;
    LCEventAction *theEventAction = 0;

    if(!ui){
        // batch mode
        theRunAction = new LCRunAction(1);
        theEventAction = new LCEventAction(1);
    }
    else{
        // interactive mode. Cant call ROOT in visualisation, or its crashes on BeamOn
        theRunAction = new LCRunAction(0);
        theEventAction = new LCEventAction(0);
    }


    runManager->SetUserAction(theRunAction);
    runManager->SetUserAction(theEventAction);

    G4VisManager *visManager = new G4VisExecutive("0");
    visManager->Initialise();

    G4HadronicProcessStore::Instance()->SetVerbose(0); 
    G4ProductionCutsTable::GetProductionCutsTable()->SetVerboseLevel(0);

    G4UImanager *uiManager = G4UImanager::GetUIpointer();

    if(!ui){
        // batch mode
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        uiManager->ApplyCommand(command+fileName);
    }
    else{
        // interactive mode
        uiManager->ApplyCommand("/control/macroPath /home/FoxWise/Documents/FCAL/LUCAS/");
        uiManager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }


    delete visManager;
    delete runManager;

    return 0;
}
