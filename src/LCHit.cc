#include "LCHit.hh"

LCHit::LCHit(G4double x, G4double y, G4double z,
             G4int sector, G4int pad, G4int layer,
             G4double energy, G4int direction, G4int bornInSi, G4int pdg):G4VHit(),
            fHit_x(x), fHit_y(y), fHit_z(z),
            fSector(sector), fPad(pad), fLayer(layer),
            fEnergy(energy), fDirection(direction), fBornInSi(bornInSi), fPDG(pdg){}

LCHit::~LCHit(){}