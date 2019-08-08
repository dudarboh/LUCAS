#include "LCHit.hh"

G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator = 0;

LCHit::LCHit(G4int sector, G4int pad, G4int layer)
    :G4VHit(),
    hit_sector(sector),
    hit_pad(pad),
    hit_layer(layer),
    hit_bs(-999),
    hit_energy(0.){}

LCHit::~LCHit(){}
