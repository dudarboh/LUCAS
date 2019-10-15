#include "LCTrHit.hh"

G4ThreadLocal G4Allocator<LCTrHit>* LCTrHitAllocator = 0;


// Hit is the energy deposit in a pad with a certain sector, pad, layer and hit_energy.
LCTrHit::LCTrHit(G4int s, G4int p, G4int l):G4VHit(), sector(s),
                                                      pad(p),
                                                      layer(l),
                                                      type(-1),
                                                      energy(0.){}

LCTrHit::~LCTrHit(){}
