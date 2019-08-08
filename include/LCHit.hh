#ifndef LCHIT_HH_
#define LCHIT_HH_ 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Threading.hh"


class LCHit:public G4VHit{
    public:
    LCHit(G4int sector, G4int pad, G4int layer);

        virtual ~LCHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        inline void AddHitEnergy(G4double energy);

        G4int hit_sector, hit_pad, hit_layer, hit_bs;
        G4double hit_energy;
};  


// You just need it
typedef G4THitsCollection<LCHit> LCHitsCollection;

extern G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator;

inline void* LCHit::operator new(size_t){
    if(!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    void *hit;
    hit = (void*)LCHitAllocator->MallocSingle();
    return hit;
}

inline void LCHit::operator delete(void *hit){
    if(!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    LCHitAllocator->FreeSingle((LCHit*)hit);
}

inline void LCHit::AddHitEnergy(G4double energy){hit_energy += energy;}

#endif
