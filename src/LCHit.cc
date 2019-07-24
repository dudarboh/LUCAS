/*
 * LCHit.cc
 * 2ModLumiCal
 *
 *  Created on: Mar 28, 2009
 *      Author: aguilar
 *
 */

#include "LCHit.hh"

#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"

#include <assert.h>
#include <map>

G4Allocator<LCHit> LCHitAllocator;

//constructor defined in the header
LCHit::LCHit(G4double pXh, G4double pYh, G4double pZh, // hit position
    G4double pXc, G4double pYc, G4double pZc, // cell position
    G4double pE, G4int pPID, G4int pPDG, // parent ID number and particle type
    cell_code pCode, // cell identifier
    G4double pTOF){ // time of flight
    
    Xhit = pXh, Yhit = pYh, Zhit = pZh;
    Xcell = pXc, Ycell = pYc, Zcell = pZc;
    TOF = pTOF;
    trackIDs = new PrimaryIDMap;
    trackIDs->clear();
    code.id0 = pCode.id0;
    code.id1 = pCode.id1;
    SetEnergy(0.0);
    NoOfContributingHits = 0;
    AddEdep(pPID, pPDG, pE);
}
//destructor
LCHit::~LCHit(){
    trackIDs->clear();
    delete trackIDs;
}

// Accumulates energy from primary particles that enter the cell.
void LCHit::AddEdep(G4int pPID, G4int pPDG, G4double de){
    NoOfContributingHits ++;
    // increment energy deposition per hit
    Energy += de;
    PrimaryIDMapIterator iter = trackIDs->find(pPID);
    if(iter == trackIDs->end()) trackIDs->insert(PrimaryIDPair(pPID, pPDG));
}
