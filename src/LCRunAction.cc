/*
 * LCRunAction.cc
 * 2ModLumiCal
 *
 *  Created on: Apr 8, 2009
 *      Author: aguilar
 */
#include <sys/times.h>
#include "LCRunAction.hh"
#include "Setup.hh"

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

    Setup::StartTime = times(&fTimeNow);
    Setup::NoOfEventsToProcess = Run->GetNumberOfEventToBeProcessed();

    // set random seed manually
    G4long seed = Setup::StartTime;
    G4RandGauss::setTheSeed(seed);
    G4cout<<" Starting with random seed "<<seed<<G4endl;
    Setup::StartRandomSeed = seed;

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
        tms fTimeNow;
        clock_t EndRunT = times(&fTimeNow);
        G4double diff = 10.*(EndRunT - Setup::StartTime)*ms ;
        G4int EventsProcessed = Run->GetNumberOfEvent(); 
        G4cout << "|                End of Run  :  "<< Run->GetRunID()<< G4endl;
        G4cout << "|                  time now  :  "<< ctime(&tnow) ;
        G4cout << "|            Events Processed:  "<< EventsProcessed<< G4endl;
        if(Setup::batchMode){
            G4cout << "|             written to file:  "<< Setup::RootFileName << G4endl;
        }
        G4cout << "|                Time elapsed:  "<< diff / s << " seconds."<< G4endl;
        G4cout << "|    Time to process an event:  "<< diff/G4double(EventsProcessed) / s << " seconds."<< G4endl;
    }
    else{
        G4cout << "|                     Begin of Run  : "<< Run->GetRunID()<< G4endl;
        G4cout << "|                         time now  : "<< ctime(&tnow) ;
        G4cout << "|                      Random Seed  : "<< Setup::StartRandomSeed << G4endl;
        G4cout << "| Global Parameters for the Run are : "           <<G4endl;
        G4cout << "---------------------------------------------------------------------"<<G4endl;
        G4cout << "|                   batchMode:  "<< Setup::batchMode << G4endl;
        G4cout << "|                   macroName:  "<< Setup::macroName << G4endl;
        G4cout << "|                  PrintLevel:  "<< Setup::PrintLevel << G4endl;
        G4cout << "|           Logging frequency:  "<< Setup::LogFreq << G4endl;
    
        if(Setup::batchMode){
            G4cout << "|       ROOT output file name:  "<< Setup::RootFileName << G4endl;
            G4cout << "|       ROOT output open mode:  "<< Setup::RootFileMode << G4endl;
        }
        if(Setup::LcalTBeam) G4cout << "|             The senrio is  :  "<< Setup::TBeam_senrio << G4endl;
        G4cout << "|                   SetupFile:  "<< Setup::SetupFile << G4endl;
        G4cout << "|         Nominal field value:  "<< Setup::Nominal_Field_value / tesla << " [T]"<< G4endl;
        G4cout << "| Number of events to process:  "<< Setup::NoOfEventsToProcess << G4endl; 
        G4cout << "|   Detector components build:  "<< G4endl;
        G4cout << "|                   Beam Tube:  "<< Setup:: Build_Beampipe << G4endl;
        G4cout << "|                        LCAL:  "<< Setup:: Build_LCal << G4endl;
        G4cout << "|                       LHCAL:  "<< Setup:: Build_LHcal << G4endl;
        G4cout << "|                        BCAL:  "<< Setup:: Build_BCal << G4endl;
        G4cout << "|                        MASK:  "<< Setup:: Build_Mask << G4endl;
        G4cout << "|                    rangeCut:  "<< Setup::rangeCut/ mm << " [mm]"<<G4endl;
        G4cout << "|   Region Production Cuts:     "<< G4endl;
        G4cout << "|                        LCAL:  "<<  Setup::LCal_Region_Cut / mm <<" [mm]"<< G4endl;
        G4cout << "|                       LHCAL:  "<<  Setup::LHcal_Region_Cut / mm <<" [mm]"<< G4endl;
        G4cout << "|                        BCAL:  "<<  Setup::BCal_Region_Cut / mm <<" [mm]"<< G4endl;
        G4cout << "|                        MASK:  "<<  Setup::Mask_Region_Cut / mm <<" [mm]"<< G4endl;
    }
    G4cout << "========================================================================"<<G4endl;
}
