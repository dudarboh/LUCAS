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
    G4cerr << " LUCAS [-m macro ] [-u UIsession] [-o outputRootFile] [-r gamma] [-p physListName]" << G4endl;
    G4cerr << " -m (macro) file to execute" << G4endl;
    G4cerr << " -u UI session. Usually [ Qt, Xm, GAG, tcsh, csh ]. Didn't work for me. Leave empty for the default one" << G4endl;
    G4cerr << " -o name of the output root file" << G4endl;
    G4cerr << " -r run scenario e/gamma. Places the Cu target and turns on the magnet if gamma" << G4endl;
    G4cerr << " -p physics list name" << G4endl;
  }
}

int main(int argc, char** argv){
    // std::cout<<"Start of main"<<std::endl;

    if ( argc > 11 ) {
        PrintUsage();
        return 1;
    }

    G4String macro;
    G4String session;
    G4String output = "lucas";
    G4String run = "e";
    G4String physListName;
    for ( G4int i=1; i<argc; i=i+2 ) {
        if ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
        else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
        else if ( G4String(argv[i]) == "-o" ) output = argv[i+1];
        else if ( G4String(argv[i]) == "-r" ) run = argv[i+1];
        else if ( G4String(argv[i]) == "-p" ) physListName = argv[i+1];
        else {
            PrintUsage();
            return 1;
        }
    }
    if(run == "gamma") ph_run=true;
    else if (run != "e"){
        PrintUsage();
        return 1;
    }
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
    G4VModularPhysicsList* physicsList = nullptr;

    // Check if the name is known to the factory
    if ( physListName.size() &&  (! factory.IsReferencePhysList(physListName) ) ) {
        PrintUsage();
        G4cerr << "Physics list " << physListName << " is not available in PhysListFactory." <<"Try FTFP_BERT_EMV/EMX/EMY/EMZ"<< G4endl;
        return 1;
    }

    // If name is not defined use FTFP_BERT
    if ( ! physListName.size() ) physListName = "FTFP_BERT";

    // Reference PhysicsList via its name
    physicsList = factory.GetReferencePhysList(physListName);
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
