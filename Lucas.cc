//============================================================================
// Name        : LumiCalJA.cpp
// Author      : Jonathan Aguilar
// Version     : 1
// Copyright   :
// Description : LumiCal stand-alone application for Geant4
//============================================================================

#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>

#include "G4RunManager.hh"

#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"

#include "LCDetectorConstruction.hh"

#include "QGSP_BERT.hh"

#include "LCRunAction.hh"

#include "G4SystemOfUnits.hh"

#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <iostream>
using namespace std;

int main(int argc, char** argv){
    tms fStartTime;
    clock_t StartTime = times(&fStartTime); // in miliseconds 

    G4UIExecutive* ui = 0;
    if(argc == 1) ui = new G4UIExecutive(argc, argv);

    G4RunManager *runManager = new G4RunManager;

    // Detectron construction and physics list
    runManager->SetUserInitialization(new LCDetectorConstruction);

    G4VUserPhysicsList *physicsList = new QGSP_BERT;
    physicsList->SetVerboseLevel(1);
    physicsList->SetDefaultCutValue(0.005*mm);
    runManager->SetUserInitialization(physicsList);
 
    // User Action Classes
    runManager->SetUserAction(new PrimaryGeneratorAction);

    LCRootOut *theRootOut = 0;
    LCRunAction *theRunAction = 0;
    LCEventAction *theEventAction = 0;

    if(!ui){
        // batch mode
        theRootOut = new LCRootOut();
        theRunAction = new LCRunAction(theRootOut);
        theEventAction = new LCEventAction(theRootOut);
    }
    else{
        // interactive mode. Cant call ROOT in visualisation, or its crashes on BeamOn
        theRunAction = new LCRunAction();
        theEventAction = new LCEventAction();
    }



    runManager->SetUserAction(theRunAction);
    runManager->SetUserAction(theEventAction);
    runManager->SetUserAction(new SteppingAction(theEventAction));


    G4VisManager *visManager = new G4VisExecutive;

    visManager->Initialize();

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

    time_t now;
    time(&now);
    tms fEndTime;
    
    clock_t EndTime = times(&fEndTime);
    G4double diff = 10.*( EndTime - StartTime )*ms ; 
    
    G4cout<<"End Job - time elapsed: " <<diff / s<<" seconds"<<G4endl; 
    G4cout<<"\n**** "<<" run ended  at "<<ctime(&now)<<G4endl;
    return 0;
}
