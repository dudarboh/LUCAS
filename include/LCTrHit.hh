#ifndef LCTrHit_h
#define LCTrHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

#include <vector>

class LCTrHit:public G4VHit{
    public:
        LCTrHit(G4int sector, G4int pad, G4int layer);

        virtual ~LCTrHit();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        // Adds energy to the pad
        void AddHitEnergy(G4double energy);

        G4int hit_sector, hit_pad, hit_layer, hit_n_particles;
        G4double hit_energy;
        G4double hit_length;

        std::vector<G4double> particle_x;
        std::vector<G4double> particle_y;
        std::vector<G4double> particle_z;
        std::vector<G4int> particle_type;
        std::vector<G4double> particle_px;
        std::vector<G4double> particle_py;
        std::vector<G4double> particle_pz;
        std::vector<G4double> particle_energy;
        std::vector<G4int> particle_primary;

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
