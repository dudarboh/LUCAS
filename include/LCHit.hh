#ifndef LCHit_h
#define LCHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"
#include "G4ThreeVector.hh"

class LCHit:public G4VHit{
    public:
        LCHit(G4int s, G4int p, G4int l);

        virtual ~LCHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int fSector, fPad, fLayer;
        G4double fTotalEdep, fTotalTrackLen;
        G4int fIsPrimary;
        std::vector <G4int> fMcContId{};
        std::vector <G4ThreeVector> fMcContPos{};
        std::vector <G4ThreeVector> fMcContMom{};
        std::vector <G4double> fMcContEnergy{};
        std::vector <G4double> fMcContEdep{};
        std::vector <G4double> fMcContTrackLen{};
        std::vector <G4int> fMcContPdg{};
};

// You just need this part. Dont change it
using LumicalHC = G4THitsCollection<LCHit>;

extern G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator;

inline void* LCHit::operator new(size_t){
    if(!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    void *hit;
    hit = (void *) LCHitAllocator->MallocSingle();
    return hit;
}

inline void LCHit::operator delete(void *hit){
    if (!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    LCHitAllocator->FreeSingle((LCHit*)hit);
}

#endif
