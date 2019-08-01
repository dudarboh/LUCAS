#ifndef LCHIT_HH_
#define LCHIT_HH_ 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4Threading.hh"


class LCHit:public G4VHit{
    public:
        LCHit(G4double x, G4double y, G4double z,
              G4int sector, G4int pad, G4int layer,
              G4double energy, G4int direction, G4int pdg);

        virtual ~LCHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        inline G4double GetX() const {return fHit_x;}
        inline G4double GetY() const {return fHit_y;}
        inline G4double GetZ() const {return fHit_z;}
        inline G4int GetSector() const {return fSector;}
        inline G4int GetPad() const {return fPad;}
        inline G4int GetLayer() const {return fLayer;}
        inline G4double GetEnergy() const {return fEnergy;}
        inline G4int GetDirection() const {return fDirection;}
        inline G4int GetPDG() const {return fPDG;}

    private:
        G4double fHit_x, fHit_y, fHit_z;
        G4int fSector, fPad, fLayer;
        G4double fEnergy;
        G4int fDirection, fPDG;
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

#endif
