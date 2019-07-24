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
#include "LCEventAction.hh"
#include "LCRootOut.hh"
#include "Setup.hh"

#include "G4SystemOfUnits.hh"

#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include <iostream>
using namespace std;

int main(int argc, char* argv[]){
  
    LCRootOut *theLCRootOut = NULL;
    G4RunManager *runManager = new G4RunManager;

    Setup *theSetup = Setup::GetSetup();
    theSetup->SetupInit(argc, argv);

    // Detectron construction and physics list
    runManager->SetUserInitialization(new LCDetectorConstruction);

    G4VUserPhysicsList *theList = new QGSP_BERT;
    theList->SetDefaultCutValue(Setup::rangeCut);
    runManager->SetUserInitialization(theList);
 
    tms fStartTime;
    clock_t StartTime = times(&fStartTime); // in miliseconds 

    runManager->Initialize();

    // User Action Classes
    runManager->SetUserAction(new PrimaryGeneratorAction);

    LCRootOut *theRootOut = new LCRootOut();
    LCRunAction *theRunAction;
    LCEventAction *theEventAction;

    // Batch mode is false by default
    if(Setup::batchMode){
        theRunAction = new LCRunAction(theRootOut);
        theEventAction = new LCEventAction(theRootOut);
    }
    else{
        theRunAction = new LCRunAction();
        theEventAction = new LCEventAction();
    }

    runManager->SetUserAction(theRunAction);
    runManager->SetUserAction(theEventAction);
    runManager->SetUserAction(new SteppingAction(theEventAction));

    G4UIExecutive *ui;
    if(Setup::batchMode) ui = 0;
    else ui = new G4UIExecutive(argc, argv);

    G4VisManager *visManager = new G4VisExecutive;
    visManager->Initialize();
    G4UImanager *uiManager = G4UImanager::GetUIpointer();

    G4cout<<"Setup::batchMode: "<<Setup::batchMode<<G4endl;
    G4cout<<"/control/execute " + Setup::macroName<<G4endl;

    if(Setup::batchMode){
        G4String command = "/control/execute ";
        uiManager->ApplyCommand(command + Setup::macroName);
    }
    else{
        uiManager->ApplyCommand("/control/execute init_vis.mac");

        G4String command = "/control/execute ";
        if(Setup::macroName != "") uiManager->ApplyCommand(command + Setup::macroName);

        ui->SessionStart();
        delete ui;

        delete visManager;
    }

    delete runManager;

    time_t now;
    time(&now);
    tms fEndTime;
    
    clock_t EndTime = times(&fEndTime);
    G4double diff = 10.*( EndTime - StartTime )*ms ; 
    
    G4cout<<"End Job - time elapsed: " <<diff / s<<" seconds"<<G4endl; 
    delete theSetup;
    G4cout<<"\n**** "<<" run ended  at "<<ctime(&now)<<G4endl;
    return 0;
}
