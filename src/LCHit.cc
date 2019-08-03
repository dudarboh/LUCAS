#include "LCHit.hh"

G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator = 0;

LCHit::LCHit(G4int sector, G4int pad, G4int layer,
             G4double energy):G4VHit(),
            fSector(sector), fPad(pad), fLayer(layer), fEnergy(energy){}

LCHit::~LCHit(){}
