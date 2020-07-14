#ifndef LCHitTracker_h
#define LCHitTracker_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCHitTracker:public G4VHit{
    public:
        LCHitTracker(G4int s, G4int p, G4int l);

        virtual ~LCHitTracker();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int sector, pad, layer, type;
        G4double energy, track_len;
        G4double x, y, z, px, py, pz, p_energy;
};

// You just need this part. Dont change it
using LCTrackerHC = G4THitsCollection<LCHitTracker>;

extern G4ThreadLocal G4Allocator<LCHitTracker>* LCHitTrackerAllocator;

inline void* LCHitTracker::operator new(size_t){
    if(!LCHitTrackerAllocator) LCHitTrackerAllocator = new G4Allocator<LCHitTracker>;
    return (void*)LCHitTrackerAllocator->MallocSingle();
}

inline void LCHitTracker::operator delete(void *hit){
    LCHitTrackerAllocator->FreeSingle((LCHitTracker*)hit);
}

#endif
