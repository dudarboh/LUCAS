#ifndef LCHit_h
#define LCHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCHit:public G4VHit{
    public:
        LCHit(G4int sector, G4int pad, G4int layer);

        virtual ~LCHit();

        // operators
        const LCHit& operator=(const LCHit&);
        G4bool operator==(const LCHit&) const;

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        // Adds energy to the pad
        void AddHitEnergy(G4double energy);
        // Marks if pad was crossed by Primary particle
        void SetIsPrimary();

        // Get protected variables
        G4int GetSector() const;
        G4int GetPad() const;
        G4int GetLayer() const;
        G4int GetIsPrimary() const;
        G4double GetEnergy() const;
    private:
        G4int hit_sector, hit_pad, hit_layer;
        G4int is_primary;
        G4double hit_energy;
};  

// You just need this part. Dont change it
typedef G4THitsCollection<LCHit> LCHitsCollection;

extern G4ThreadLocal G4Allocator<LCHit>* LCHitAllocator;

inline void* LCHit::operator new(size_t){
    if(!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    return (void*)LCHitAllocator->MallocSingle();
}

inline void LCHit::operator delete(void *hit){
    if(!LCHitAllocator) LCHitAllocator = new G4Allocator<LCHit>;
    LCHitAllocator->FreeSingle((LCHit*)hit);
}

// Adds energy
inline void LCHit::AddHitEnergy(G4double energy){hit_energy += energy;}
// Marks as primary
inline void LCHit::SetIsPrimary(){is_primary = 1;}

// Get methods
inline G4int LCHit::GetSector() const {return hit_sector;}
inline G4int LCHit::GetPad() const {return hit_pad;}
inline G4int LCHit::GetLayer() const {return hit_layer;}
inline G4int LCHit::GetIsPrimary() const {return is_primary;}
inline G4double LCHit::GetEnergy() const {return hit_energy;}

#endif
