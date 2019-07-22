/*
 * LCEventAction.hh
 * 2ModLumiCal
 *
 *  Created on: Apr 4, 2009
 *      Author: Jonathan Aguilar and Bogdan Pawlik
 *
 *      Modeled after Geant4 extended example RE01 event action class.
 *      Use this class to write data files
 */

#ifndef LCEVENTACTION_HH_
#define LCEVENTACTION_HH_

#include "LCSensitiveDetector.hh" // need to get the SD name
#include "LCHit.hh"
#include "LCRootOut.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <stdio.h>
#include <time.h>


class G4PrimaryParticle;

class LCEventAction : public G4UserEventAction
{
public:
    LCEventAction(LCRootOut *RO);
    LCEventAction();
    ~LCEventAction();

public:
       void BeginOfEventAction(const G4Event* );
       void EndOfEventAction(const G4Event* );
       void AddLeakEnergy ( G4double elost );
       G4double GetLeakEnergy();

private:
    G4int collID;       // ID number for a collection of calorimeter hits
    G4String collName;  // hits collection name
    LCRootOut *RootOut; // Handles writing the ROOT tree
    G4SDManager *SDman;
    time_t  _start, _end;
    G4int noTrackKilled;
    G4double LeakEnergy;  // Accumulates energy leak
};

#endif /* LCEVENTACTION_HH_ */
