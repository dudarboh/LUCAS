#include "LCHitTracker.hh"

G4ThreadLocal G4Allocator<LCHitTracker>* LCHitTrackerAllocator = 0;

LCHitTracker::LCHitTracker(G4int s, G4int p, G4int l) :G4VHit(),
                                                        sector(s),
                                                        pad(p),
                                                        layer(l),
                                                        type(-1),
                                                        energy(0.),
                                                        track_len(0.),
                                                        p_energy(0.){}

LCHitTracker::~LCHitTracker(){}
