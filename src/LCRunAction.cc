
#include <sys/times.h>
#include "LCRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "G4HCtable.hh"
#include "Randomize.hh"

#include "G4SystemOfUnits.hh"


LCRunAction::LCRunAction(LCRootOut *RO){RootOut = RO;}


LCRunAction::LCRunAction(){RootOut = 0;}

LCRunAction::~LCRunAction(){;}


void LCRunAction::BeginOfRunAction(const G4Run* Run){
    tms fTimeNow;

    G4cout<<"Run "<<Run->GetRunID()<<" start."<<G4endl;
    G4cout<<" You may stop the run gently, any time you wish."<<G4endl;
    G4cout<<" To do it, create in the current directory semaphore file "<<G4endl;
    G4cout<<" named <aStopRun> , file content is ignored "<<G4endl;
    G4cout<<" ( eg. shell> touch aStopRun  )"<<G4endl;

    clock_t StartTime = times(&fTimeNow);

    // set random seed manually
    G4long seed = StartTime;
    G4RandGauss::setTheSeed(seed);
    G4cout<<" Starting with random seed "<<seed<<G4endl;

    if(RootOut) RootOut->Init();
    Print("RUN_BEGIN", Run);
}

void LCRunAction::EndOfRunAction(const G4Run* Run){
    Print("END_OF_RUN", Run);
    if(RootOut) RootOut->End();
}

void LCRunAction::Print(G4String now, const G4Run* Run){
    time_t tnow = time(NULL);
    G4cout << "======================================================================"<<G4endl;
    if(now == "END_OF_RUN"){
        G4int EventsProcessed = Run->GetNumberOfEvent(); 
        G4cout << "|                End of Run  :  "<< Run->GetRunID()<< G4endl;
        G4cout << "|                  time now  :  "<< ctime(&tnow) ;
        G4cout << "|            Events Processed:  "<< EventsProcessed<< G4endl;
    }
    else{
        G4cout << "|                     Begin of Run  : "<< Run->GetRunID()<< G4endl;
        G4cout << "|                         time now  : "<< ctime(&tnow) ;
        G4cout << "| Global Parameters for the Run are : "           <<G4endl;
        G4cout << "---------------------------------------------------------------------"<<G4endl;
    G4cout << "========================================================================"<<G4endl;
    }
}