// Change this to turn on/off multithreaded mode
#ifdef G4MULTITHREADED
    #include "G4MTRunManager.hh"
#else
    #include "G4RunManager.hh"
#endif

#include "LCRunAction.hh"
#include "LCDetectorConstruction.hh"
#include "LCActionInitialization.hh"
#include "LCPrimaryGeneratorAction.hh"
#include "LCEventAction.hh"
#include "LCSensitiveDetector.hh"
#include "LCHit.hh"

#include "G4PhysListFactory.hh"

#include "G4UIExecutive.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"


int main(int argc, char** argv){
    G4Random::setTheEngine(new CLHEP::RanecuEngine);

    G4UIExecutive* ui = 0;
    if(argc == 1) ui = new G4UIExecutive(argc, argv);

    #ifdef G4MULTITHREADED
        G4MTRunManager *runManager = new G4MTRunManager;
        runManager->SetNumberOfThreads(1);
    #else
        G4RunManager *runManager = new G4RunManager;
    #endif

    LCDetectorConstruction *detector = new LCDetectorConstruction;
    runManager->SetUserInitialization(detector);

    G4PhysListFactory factory;
    G4VModularPhysicsList *physicsList = factory.GetReferencePhysList("QGSP_BERT");
    // G4VModularPhysicsList *physicsList = factory.GetReferencePhysList("QGSP_BERT_EMY");
    // G4VModularPhysicsList *physicsList = factory.GetReferencePhysList("QGSP_BERT_EMZ");
    physicsList->SetDefaultCutValue(0.005*mm);
    runManager->SetUserInitialization(physicsList);

    LCActionInitialization *action = new LCActionInitialization;
    runManager->SetUserInitialization(action);

    G4VisManager *visManager = new G4VisExecutive;
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
        uiManager->ApplyCommand("/control/macroPath /home/FoxWise/Documents/FCAL/LUCAS");
        uiManager->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    }

    delete visManager;
    delete runManager;
}
