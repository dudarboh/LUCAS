#ifndef LCTrHit_h
#define LCTrHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCTrHit:public G4VHit{
    public:
        LCTrHit(G4int s, G4int p, G4int l);

        virtual ~LCTrHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int sector, pad, layer, type;
        G4double energy;

        G4double particle_x;
        G4double particle_y;
        G4double particle_z;
        G4double particle_px;
        G4double particle_py;
        G4double particle_pz;
        G4double particle_energy;
};  

// You just need this part. Dont change it
typedef G4THitsCollection<LCTrHit> LCTrHitsCollection;

extern G4ThreadLocal G4Allocator<LCTrHit>* LCTrHitAllocator;

inline void* LCTrHit::operator new(size_t){
    if(!LCTrHitAllocator) LCTrHitAllocator = new G4Allocator<LCTrHit>;
    return (void*)LCTrHitAllocator->MallocSingle();
}

inline void LCTrHit::operator delete(void *hit){
    if(!LCTrHitAllocator) LCTrHitAllocator = new G4Allocator<LCTrHit>;
    LCTrHitAllocator->FreeSingle((LCTrHit*)hit);
}

#endif
