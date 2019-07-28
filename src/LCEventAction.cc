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
#include <fstream>
#include "LCEventAction.hh"

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


LCEventAction::LCEventAction(LCRootOut *RO){
    // initialize the collection ID number
    collID = -1;
    RootOut = RO;
}


LCEventAction::LCEventAction(){
    // initialize the collection ID number
    collID = -1;
    RootOut = 0;
}

LCEventAction::~LCEventAction(){}


void LCEventAction::BeginOfEventAction(const G4Event*){

}

void LCEventAction::EndOfEventAction(const G4Event* event){
    // Use a Sensitive Detector manager to assign an ID # to the hits collections
    if(collID<0) collID = G4SDManager::GetSDMpointer()->GetCollectionID("SDHitsCollection");
    if(collID<0) G4cerr<<"WATAFACK???"<<endl;

    G4HCofThisEvent *HCE = event->GetHCofThisEvent();

    LCHitsCollection *HitsCollection = 0;
  
    if(HCE) HitsCollection = (LCHitsCollection*)(HCE->GetHC(collID));

    // fill the ROOT Tree
    if(HitsCollection){
        if(RootOut) RootOut->ProcessEvent(event, HitsCollection);
    }
}
