#include "LCHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator = 0;

LCHit::LCHit(G4int sector, G4int pad, G4int layer, G4double energy):G4VHit(),
            fXcell(sector), fYcell(pad), fZcell(layer),
            fEnergy(energy), fNoOfContributingHits(1){}

LCHit::~LCHit(){}

LCHit::LCHit(const LCHit& right):G4VHit(){
    fXhit = right.fXhit;
    fYhit = right.fYhit;
    fZhit = right.fZhit;
    fXcell = right.fXcell;
    fYcell = right.fYcell;
    fZcell = right.fZcell;
    fEnergy = right.fEnergy;
    fNoOfContributingHits = right.fNoOfContributingHits;
}

const LCHit& LCHit::operator=(const LCHit& right){
    fXhit = right.fXhit;
    fYhit = right.fYhit;
    fZhit = right.fZhit;
    fXcell = right.fXcell;
    fYcell = right.fYcell;
    fZcell = right.fZcell;
    fEnergy = right.fEnergy;
    fNoOfContributingHits = right.fNoOfContributingHits;

    return *this;
}

G4int LCHit::operator==(const LCHit& right) const{
  
  return (fXcell == right.fXcell) && (fYcell == right.fYcell) && (fZcell == right.fZcell);
}

void LCHit::Print(){
    G4cout
    <<"Edep: "<<std::setw(7)<<G4BestUnit(fEnergy, "Energy")
    <<"Xhit: "<<std::setw(7)<<G4BestUnit(fXhit, "Length")
    <<"Yhit: "<<std::setw(7)<<G4BestUnit(fYhit, "Length")
    <<"Zhit: "<<std::setw(7)<<G4BestUnit(fZhit, "Length")
    <<"Xcell: "<<std::setw(7)<<G4BestUnit(fXcell, "Length")
    <<"Ycell: "<<std::setw(7)<<G4BestUnit(fYcell, "Length")
    <<"Zcell: "<<std::setw(7)<<G4BestUnit(fZcell, "Length")
    <<"fNoOfContributingHits: "<<std::setw(7)<<fNoOfContributingHits
    << G4endl;
}