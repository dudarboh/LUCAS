/*
 * LCSensitiveDetector.hh
 * 2ModLumiCal
 *
 *  Created on: Mar 26, 2009
 *      Author: Jonathan Aguilar
 *
 *      Borrows heavily from Geant4 novice example 4 (ExN04)
 *      and LumiCalSD, written by Bogdan Pawlik
 */

#ifndef LCSENSITIVEDETECTOR_HH_
#define LCSENSITIVEDETECTOR_HH_

// time optimization
#include <time.h>

#include "LCHit.hh"

#include "G4SDManager.hh"
#include "G4TouchableHandle.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4NavigationHistory.hh"
#include "G4TouchableHistory.hh"
#include "G4PVReplica.hh"
#include "G4VSensitiveDetector.hh"
#include "Setup.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

//typedef G4THitsCollection<LCHit> LCHitsCollection; - defined in LCHit
typedef std::map<G4int,LCHit*> LCHitMap;
typedef LCHitMap::iterator LCHitMapIterator;
typedef std::pair<G4int,LCHit*> LCHitMapPair;


class LCSensitiveDetector : public G4VSensitiveDetector
{
public:

    LCSensitiveDetector(G4String sdname,        // name of SD
                        G4double CalRhoMin,     // LumiCal inner radius
                        G4double PhiOffset,     // phi angle offset
                        G4double gridRho,       // cell # in sector
                        G4double gridPhi,       // sector # in layer
                        G4int    nCellRho,      // total # cells along radius
                        G4int    nCellPhi,
			G4bool   cellvirtual);     // total # cells along phi
    ~LCSensitiveDetector();


    // Initialize a hit collection, increment the energy counter and
    // register the hit volumes
    void   Initialize(G4HCofThisEvent *HCE);
    G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
    void   EndOfEvent(G4HCofThisEvent *HCE);
    void   clear();

    // Return the collection name, so you don't have to hard-code it in
    inline G4String GetCollName() {return collName;}

    // not sure about the next 3
    void DrawAll();
    void PrintAll();
    void SaveAll();

private:
	G4String collName; // the name of the collection - "LumiCalSD_HC"

    // Set methods for SD primary parameters - the volumes that make up LC
    void SetNCellPhi(G4int nx);
    void SetNCellRho( G4int ny);
    void SetRhoCellDim(G4double c1);
    void SetPhiCellDim( G4double c2);
    void SetRhoMin(G4double c);
    void SetPhiOffset(G4double phi);

    // find if hit exists already and ++edep if it does
    // don't think I need TID
    G4bool FindHit(G4int, G4double, G4int TID, G4int pdg);

    // primary particle id
    G4int primaryID;
    // primary particle PDG code
    G4int primaryPDG;

    // hits collection id
    G4int HCID;

    // name of SD - how do you get this?
    G4String SDName;


    // hits collection
    LCHitsCollection *hitsColl;

    // map of hits
    LCHitMap *hitMap;

    // origin point from transformations = 0, 0, 0
    G4ThreeVector origin;

    // num cells in xy-directions
    G4int NumCellsRho, NumSectorsPhi;

      // grid size
  G4double cellDimRho, cellDimPhi;

      // inner radius and phi offset size
  G4double CalRhoMin, CalPhiOffset;

      // type of cell ( virtual =true -> segmentaion at SD driver level )
  G4bool VirtualCell;
     
};

#endif /* LCSENSITIVEDETECTOR_HH_ */
