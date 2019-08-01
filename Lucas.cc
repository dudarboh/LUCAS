#include "LCDetectorConstruction.hh"
#include "LCEventAction.hh"
#include "LCEventData.hh"
#include "LCHit.hh"
#include "LCPrimaryGeneratorAction.hh"
#include "LCRunAction.hh"
#include "LCSensitiveDetector.hh"

#include "G4UIExecutive.hh"
#include "G4RunManager.hh"
#include "QGSP_BERT.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"

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
    runManager->SetUserAction(new LCPrimaryGeneratorAction);

    LCEventData *EventData = 0;
    LCRunAction *theRunAction = 0;
    LCEventAction *theEventAction = 0;

    if(!ui){
        // batch mode
        EventData = new LCEventData();
        theRunAction = new LCRunAction(EventData);
        theEventAction = new LCEventAction(EventData);
    }
    else{
        // interactive mode. Cant call ROOT in visualisation, or its crashes on BeamOn
        theRunAction = new LCRunAction();
        theEventAction = new LCEventAction();
    }

    runManager->SetUserAction(theRunAction);
    runManager->SetUserAction(theEventAction);

    G4VisManager *visManager = new G4VisExecutive("0");
    visManager->Initialise();

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
