/*
 * LCEventAction.cc
 * 2ModLumiCal
 *
 *  Created on: Apr 5, 2009
 *      Author: Jonathan Aguilar and Bogdan Pawlik
 *
 *      Based off Geant4 extended example RE01
 *
 *      Note: need collection name to get a hits collection ID #.
 *      In this stand-alone version, there is only one hits collection.
 *      This name is HARD-CODED in.
 */
#include <sys/times.h>
#include <fstream>
#include "LCEventAction.hh"
#include "Setup.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"

#include "G4SystemOfUnits.hh"


LCEventAction::LCEventAction(LCRootOut * RO)
{
    // initialize the collection ID number
    collID = -1;

    RootOut = RO;
}
LCEventAction::LCEventAction()
{
    // initialize the collection ID number
    collID = -1;

    RootOut = 0;
}

LCEventAction::~LCEventAction()
{

}

void LCEventAction::AddLeakEnergy ( G4double eleak )
{
  noTrackKilled++;
  LeakEnergy += eleak;
}
G4double LCEventAction::GetLeakEnergy (){ return LeakEnergy ;}


void LCEventAction::BeginOfEventAction(const G4Event*)
{
  // reset energy counter
   LeakEnergy =0.;
   noTrackKilled = 0;
   //
    // Check if the semaphore file exists, if so abort gently the run

     std::ifstream semaphoreFile("aStopRun");
     if( semaphoreFile ) {
       G4RunManager::GetRunManager()->AbortRun( false ); 
       semaphoreFile.close();
       G4cout<< " LCEventAction::BeginOfEventAction : Aborting the run on user request !" << G4endl;  
    // remove file to avoid unwanted abort of the next run
       G4String rmCmd = "/control/shell rm aStopRun";
       G4UImanager::GetUIpointer()->ApplyCommand(rmCmd); 

     }

    // Use a Sensitive Detector manager to assign an ID #
    // to the hits collections

    SDman = G4SDManager::GetSDMpointer();
    time(&_start);

    if (collID < 0) {
        // there is only 1 hits collection, so the name is constant
        collID = SDman->GetCollectionID("LumiCalSD_HC");
    }

}

void LCEventAction::EndOfEventAction(const G4Event* event)
{
    // Check that a collection ID has been assigned
    if (collID < 0) { return; }
    //
    G4int evtnum = (event->GetEventID())+1;
    //
    // report on track killed
    //
    if ( noTrackKilled > 0 ){
      G4cout << " Event: "<<evtnum + Setup::EventStartNumber 
             <<" Back Energy Leak : "<<LeakEnergy / GeV <<" GeV"<<G4endl;
    }
    //
    // Log Run progress
    //
    if ( Setup::LogFreq > 0 ) 
      {
	if ( !(evtnum%(Setup::LogFreq)))
	  {
	time_t now ; time(&now);
      tms fTimeEnd;
      clock_t fNow = times( &fTimeEnd );
      G4double tdiff = 10.*(fNow - Setup::StartTime) *ms ;
      G4double evtime = tdiff / (G4double)(evtnum);
      G4double evtleft = G4double(Setup::NoOfEventsToProcess - evtnum );
      G4double EST = evtime*evtleft;
      evtleft /= (G4double)Setup::NoOfEventsToProcess;
      evtleft  = (1. - evtleft)*100.;
      G4cout << G4endl;
      G4cout<<" LCEventAction::EndOfEventAction: "<<G4endl;
      G4cout<<" Event number : "<<evtnum + Setup::EventStartNumber <<" - "<<evtleft<<"% done !"<<"  current time is: "<< ctime(&now);
      G4cout<<"       time elapsed: "<<tdiff / s << "  time/event: "<< evtime/s <<"  EST: "<< EST/s << G4endl;
 
	  }
      }

    //
    G4HCofThisEvent *HCE = event->GetHCofThisEvent();

    LCHitsCollection *HitsColl = 0;
  
    if (HCE) { HitsColl = (LCHitsCollection*)(HCE->GetHC(collID)); }

    if (HitsColl) { // fill the ROOT Tree

      if( RootOut ) {
	if ( Setup::AccumulateEvents ){
	  RootOut->ProcEventAccumulate( HitsColl );
	}
	else {
	  RootOut->ProcessEvent( event, HitsColl );
	}
      }
    }
}
