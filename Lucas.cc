#include "LCDetectorConstruction.hh"
#include "LCActionInitialization.hh"
#include "G4RunManager.hh"

#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

bool ph_run=false;

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " LUCAS [-m macro ] [-u UIsession] [-o outputRootFile] [-ph off/on]" << G4endl;
  }
}

int main(int argc, char** argv){
    // std::cout<<"Start of main"<<std::endl;

    if ( argc > 9 ) {
        PrintUsage();
        return 1;
    }

    G4String macro;
    G4String session;
    G4String output = "lucas";
    G4String photon_run = "off";
    for ( G4int i=1; i<argc; i=i+2 ) {
        if ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
        else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
        else if ( G4String(argv[i]) == "-o" ) output = argv[i+1];
        else if ( G4String(argv[i]) == "-ph" ) photon_run = argv[i+1];
        else {
            PrintUsage();
            return 1;
        }
    }
    if(photon_run == "on") ph_run=true;
    // Detect interactive mode (if no macro provided) and define UI session
    //
    G4UIExecutive* ui = 0;
    if ( ! macro.size() ) {
        ui = new G4UIExecutive(argc, argv, session);
    }


    //Set random seeds
    // Optionally: choose a different Random engine...
    //
    // G4Random::setTheEngine(new CLHEP::RanecuEngine);
    // G4Random::setTheEngine(new CLHEP::MTwistEngine);

    // Construct the default run manager
    //
    G4RunManager* runManager = new G4RunManager;

    // Set mandatory initialization classes
    //
    auto detConstruction = new LCDetectorConstruction();
    runManager->SetUserInitialization(detConstruction);

    G4PhysListFactory factory;
    G4VModularPhysicsList *physicsList = factory.GetReferencePhysList("FTFP_BERT");
    physicsList->SetDefaultCutValue(0.001*mm);
    runManager->SetUserInitialization(physicsList);

    auto actionInitialization = new LCActionInitialization(output);
    runManager->SetUserInitialization(actionInitialization);

    // Initialize visualization
    auto visManager = new G4VisExecutive;
    // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
    // G4VisManager* visManager = new G4VisExecutive("Quiet");
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    auto UImanager = G4UImanager::GetUIpointer();

    // Process macro or start UI session
    //
    if ( macro.size() ) {
      // batch mode
      G4String command = "/control/execute ";
      UImanager->ApplyCommand(command+macro);
    }
    else  {
      // interactive mode : define UI session
      UImanager->ApplyCommand("/control/execute init_vis.mac");
      ui->SessionStart();
      delete ui;
    }

    delete visManager;
    delete runManager;
}
