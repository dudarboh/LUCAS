#include "LCHit.hh"

G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator = 0;

LCHit::LCHit(G4int sector, G4int pad, G4int layer)
    :G4VHit(),
    hit_sector(sector),
    hit_pad(pad),
    hit_layer(layer),
    is_primary(0),
    hit_energy(0.){}

LCHit::~LCHit(){}

const LCHit& LCHit::operator=(const LCHit& right){
    hit_sector = right.hit_sector;
    hit_pad = right.hit_pad;
    hit_layer = right.hit_layer;
    is_primary = right.is_primary;
    hit_energy = right.hit_energy;
    return *this;
}

G4bool LCHit::operator==(const LCHit& right) const{return this == &right;}