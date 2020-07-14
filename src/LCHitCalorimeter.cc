#include "LCHitCalorimeter.hh"

G4ThreadLocal G4Allocator<LCHitCalorimeter>* LCHitCalorimeterAllocator = 0;

LCHitCalorimeter::LCHitCalorimeter(G4int s, G4int p, G4int l)
:G4VHit(),
sector(s),
pad(p),
layer(l),
energy(0.)
{}

LCHitCalorimeter::~LCHitCalorimeter(){}
