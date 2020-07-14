#ifndef LCHitCalorimeter_h
#define LCHitCalorimeter_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCHitCalorimeter:public G4VHit{
    public:
        LCHitCalorimeter(G4int s, G4int p, G4int l);

        virtual ~LCHitCalorimeter();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int sector, pad, layer;
        G4double energy;
};

// You just need this part. Dont change it
using LCCalorimeterHC = G4THitsCollection<LCHitCalorimeter>;

extern G4ThreadLocal G4Allocator<LCHitCalorimeter>* LCHitCalorimeterAllocator;

inline void* LCHitCalorimeter::operator new(size_t){
    if(!LCHitCalorimeterAllocator) LCHitCalorimeterAllocator = new G4Allocator<LCHitCalorimeter>;
    return (void*)LCHitCalorimeterAllocator->MallocSingle();
}

inline void LCHitCalorimeter::operator delete(void *hit){
    LCHitCalorimeterAllocator->FreeSingle((LCHitCalorimeter*)hit);
}

#endif
