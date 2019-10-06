#include "LCCalHit.hh"

G4ThreadLocal G4Allocator<LCCalHit>* LCCalHitAllocator = 0;


// Hit is the energy deposit in a pad with a certain sector, pad, layer and hit_energy.
LCCalHit::LCCalHit(G4int sector, G4int pad, G4int layer)
    :G4VHit(),
    hit_sector(sector),
    hit_pad(pad),
    hit_layer(layer),
    hit_energy(0.){}

LCCalHit::~LCCalHit(){}
