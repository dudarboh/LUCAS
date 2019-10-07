#ifndef LCCalHit_h
#define LCCalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCCalHit:public G4VHit{
    public:
        LCCalHit(G4int s, G4int p, G4int l);

        virtual ~LCCalHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int sector, pad, layer;
        G4double energy;
};  

// You just need this part. Dont change it
typedef G4THitsCollection<LCCalHit> LCCalHitsCollection;

extern G4ThreadLocal G4Allocator<LCCalHit>* LCCalHitAllocator;

inline void* LCCalHit::operator new(size_t){
    if(!LCCalHitAllocator) LCCalHitAllocator = new G4Allocator<LCCalHit>;
    return (void*)LCCalHitAllocator->MallocSingle();
}

inline void LCCalHit::operator delete(void *hit){
    if(!LCCalHitAllocator) LCCalHitAllocator = new G4Allocator<LCCalHit>;
    LCCalHitAllocator->FreeSingle((LCCalHit*)hit);
}

#endif
