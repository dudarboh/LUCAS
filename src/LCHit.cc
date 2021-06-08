#include "LCHit.hh"

G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator = 0;

LCHit::LCHit(G4int s, G4int p, G4int l):G4VHit(), fSector(s), fPad(p), fLayer(l), fTotalEdep(0.), fTotalTrackLen(0.), fIsPrimary(0){}

LCHit::~LCHit(){}
