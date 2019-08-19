#ifndef LCHit_h
#define LCHit_h 1

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

        void AddHitEnergy(G4double energy);
        void AddBSParticle();
        void AddDirParticle();
        G4int GetSector() const;
        G4int GetPad() const;
        G4int GetLayer() const;
        G4int GetBS() const;
        G4int GetDir() const;
        G4double GetEnergy() const;

    private:
        G4int hit_sector, hit_pad, hit_layer, n_bs_particles, n_dir_particles;
        G4double hit_energy;
};  

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

inline void LCHit::AddHitEnergy(G4double energy){hit_energy += energy;}
inline void LCHit::AddBSParticle(){n_bs_particles ++;}
inline void LCHit::AddDirParticle(){n_dir_particles ++;}

inline G4int LCHit::GetSector() const {return hit_sector;}
inline G4int LCHit::GetPad() const {return hit_pad;}
inline G4int LCHit::GetLayer() const {return hit_layer;}
inline G4int LCHit::GetBS() const {return n_bs_particles;}
inline G4int LCHit::GetDir() const {return n_dir_particles;}
inline G4double LCHit::GetEnergy() const {return hit_energy;}

#endif
