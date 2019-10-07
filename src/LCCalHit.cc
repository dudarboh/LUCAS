#include "LCCalHit.hh"

G4ThreadLocal G4Allocator<LCCalHit>* LCCalHitAllocator = 0;


// Hit is the energy deposit in a pad with a certain sector, pad, layer and hit_energy.
LCCalHit::LCCalHit(G4int s, G4int p, G4int l)
    :G4VHit(),
    sector(s),
    pad(p),
    layer(l),
    energy(0.){}

LCCalHit::~LCCalHit(){}
