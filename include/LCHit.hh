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
#include "G4Allocator.hh"
#include "G4THitsCollection.hh"


#include "G4ThreeVector.hh"
#include "G4Circle.hh"
#include <map>

#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class LCHit:public G4VHit{
    public:
        LCHit(G4double xGlobal, G4double yGlobal, G4double zGlobal, // hit position
            G4double sector, G4double pad, G4double layer, // cell position
            G4double energy, G4int cellID);
        LCHit(const LCHit&);
        ~LCHit();

        // operators
        const LCHit& operator=(const LCHit&);
        G4int operator==(const LCHit&) const;

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        void AddEdep(G4double dE);


    private:

        G4double fXhit,fYhit,fZhit; // spatial hit coordinates
        G4double fXcell,fYcell,fZcell; // spatial cell coordinates
        G4double fEnergy; // Total energy that has accumulated in the cell
        G4int fCellID; // encoded cell id

        G4int fNoOfContributingHits; // number of particles contributing to this hit

    public:

        inline G4double GetXhit() const {return fXhit;}
        inline G4double GetYhit() const {return fYhit;}
        inline G4double GetZhit() const {return fZhit;}
        inline G4double GetXcell() const {return fXcell;}
        inline G4double GetYcell() const {return fYcell;}
        inline G4double GetZcell() const {return fZcell;}
        inline G4int GetNoContributingHits() const {return fNoOfContributingHits;}
        inline G4double GetEnergy() {return fEnergy;}
        inline G4int GetCellCode() {return fCellID;}

        inline void SetEnergy(G4double Energy) {fEnergy = Energy;}
};

typedef G4THitsCollection<LCHit> LCHitsCollection;


// You just need it
extern G4Allocator<LCHit> LCHitAllocator;

inline void* LCHit::operator new(size_t){
    void *aHit;
    aHit = (void *) LCHitAllocator.MallocSingle();
    return aHit;
}

inline void LCHit::operator delete(void *aHit){
    LCHitAllocator.FreeSingle((LCHit*) aHit);
}

#endif
