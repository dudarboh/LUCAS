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

LCHit::LCHit(G4double xGlobal, G4double yGlobal, G4double zGlobal,
            G4double sector, G4double pad, G4double layer,
            G4double energy, G4int cellID):G4VHit(),
            fXhit(xGlobal), fYhit(yGlobal), fZhit(zGlobal),
            fXcell(sector), fYcell(pad), fZcell(layer),
            fEnergy(energy), fCellID(cellID){
    
    SetEnergy(0.0);
    fNoOfContributingHits = 0;
    AddEdep(energy);
}

LCHit::LCHit(const LCHit& right):G4VHit(){
    fXhit = right.fXhit;
    fYhit = right.fYhit;
    fZhit = right.fZhit;
    fXcell = right.fXcell;
    fYcell = right.fYcell;
    fZcell = right.fZcell;
    fEnergy = right.fEnergy;
    fCellID = right.fCellID;
}

//destructor
LCHit::~LCHit(){}


// Accumulates energy from primary particles that enter the cell.
void LCHit::AddEdep(G4double dE){
    fNoOfContributingHits ++;
    fEnergy += dE;
}


const LCHit& LCHit::operator=(const LCHit& right){
    fXhit = right.fXhit;
    fYhit = right.fYhit;
    fZhit = right.fZhit;
    fXcell = right.fXcell;
    fYcell = right.fYcell;
    fZcell = right.fZcell;
    fEnergy = right.fEnergy;
    fCellID = right.fCellID;

    return *this;
}

G4int LCHit::operator==(const LCHit& right) const{
  return (this == &right);
}

