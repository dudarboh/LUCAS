/*
 *      Borrowed heavily from Geant4 novice example 4
 *      and CalHit.hh from Mokka
 *
 *      CALORIMETER hits - that means one cell stores the total energy from
 *      primary particles that enter it - ignore the tracks of secondary
 *      particles
 */
#ifndef LCHIT_HH_
#define LCHIT_HH_ 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"


class LCHit:public G4VHit{
    public:
        LCHit(G4int sector, G4int pad, G4int layer, G4double energy);
        LCHit(const LCHit&);
        virtual ~LCHit();

        // operators
        const LCHit& operator=(const LCHit&);
        G4int operator==(const LCHit&) const;

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        // methods from base class
        virtual void Draw(){}
        virtual void Print();

        void Add(G4double dE);

        inline G4double GetXhit() const {return fXhit;}
        inline G4double GetYhit() const {return fYhit;}
        inline G4double GetZhit() const {return fZhit;}
        inline G4int GetXcell() const {return fXcell;}
        inline G4int GetYcell() const {return fYcell;}
        inline G4int GetZcell() const {return fZcell;}
        inline G4int GetNoContributingHits() const {return fNoOfContributingHits;}
        inline G4double GetEnergy() const {return fEnergy;}

    private:

        G4double fXhit,fYhit,fZhit; // spatial hit coordinates
        G4int fXcell,fYcell,fZcell; // spatial cell coordinates
        G4double fEnergy; // Total energy that has accumulated in the cell
        G4int fNoOfContributingHits; // number of particles contributing to this hit

};


// You just need it
using LCHitsCollection = G4THitsCollection<LCHit>;

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

inline void LCHit::Add(G4double dE){
    fNoOfContributingHits ++;
    fEnergy += dE;
}

#endif
