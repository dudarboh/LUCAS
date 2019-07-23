/*
 * LCHit.hh
 * 2ModLumiCal
 *
 *  Created on: Mar 26, 2009
 *      Author: Jonathan Aguilar
 *
 *      Borrowed heavily from Geant4 novice example 4
 *      and CalHit.hh from Mokka
 *
 *      CALORIMETER hits - that means one cell stores the total energy from
 *      primary particles that enter it - ignore the tracks of secondary
 *      particles
 */

#ifndef LCHIT_HH_
#define LCHIT_HH_

// from CalHit
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Circle.hh"
#include <map>

// from ExN04
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"


// this is used for assigning a unique identifier to any cell that gets hit used in CalHit from Mokka.
typedef struct{int id0; int id1;} cell_code;

typedef std::map<G4int, G4int> PrimaryIDMap; // gets a PrimaryIDPair
typedef PrimaryIDMap::iterator PrimaryIDMapIterator; // searches through map
typedef std::pair<G4int,G4int> PrimaryIDPair; // parent ID and PDG code

class LCHit:public G4VHit{
    public:
        // This is the constructor used by LCSensitiveDetector.cc
        LCHit(G4double pXh, G4double pYh, G4double pZh, // hit position
            G4double pXc, G4double pYc, G4double pZc, // cell position
            G4double pE, G4int pPID, G4int pPDG, // parent ID number and particle type
            cell_code pCode, // cell identifier
            G4double pTOF) // time of flight
            : Xhit(pXh), Yhit(pYh), Zhit(pZh), // global hit coordinates
            Xcell(pXc), Ycell(pYc), Zcell(pZc), // global cell coordinates
            TOF(pTOF){
            
            trackIDs = new PrimaryIDMap;
            trackIDs->clear();
            code.id0 = pCode.id0;
            code.id1 = pCode.id1;
            SetEnergy(0.0);
            NoOfContributingHits = 0;
            AddEdep(pPID, pPDG, pE);
        }
        ~LCHit();

        void   Draw();
        void   Print();
        void   Save(FILE *oFile);
        G4bool Load(FILE *iFile);

        // Memory management
        inline void *operator new(size_t);
        inline void  operator delete(void *aHit);

        // Store cell size
        G4ThreeVector CellDim;


    public:
        // Hit location indexed by volume hierarchy and global coordinates
        inline G4double GetXhit() const {return Xhit;}
        inline G4double GetYhit() const {return Yhit;}
        inline G4double GetZhit() const {return Zhit;}
        inline G4double GetXcell() const {return Xcell;}
        inline G4double GetYcell() const {return Ycell;}
        inline G4double GetZcell() const {return Zcell;}
        inline G4int GetNoContributingHits() const {return NoOfContributingHits;}

        // Other information that needs to get outside
        inline G4double GetTOF() {return TOF;}
        inline G4double GetEnergy() {return Energy;}
        inline PrimaryIDMap *GetPID() {return trackIDs;}
        inline G4int GetCellCode() {return code.id0;}

        // Accumulate the total energy stored in the cell
        void AddEdep(G4int pPID, G4int pPDG, G4double de);

        inline void SetEnergy(G4double pEnergy) {Energy = pEnergy;}
        inline G4bool testCell(cell_code pCode) const {return ((code.id0 == pCode.id0)&&(code.id1 == pCode.id1));}

    protected:

        cell_code code; // encoded cell id
        G4double Energy; // Total energy that has accumulated in the cell

    private:

        G4int NoOfContributingHits; // number of particles contributing to this hit
        G4double Xhit,Yhit,Zhit; // spatial hit coordinates
        G4double Xcell,Ycell,Zcell; // spatial cell coordinates
        G4double TOF;
        PrimaryIDMap *trackIDs; // contributing primary track IDs

};


typedef G4THitsCollection<LCHit> LCHitsCollection;
extern G4Allocator<LCHit> LCHitAllocator;

inline void* LCHit::operator new(size_t)
{
    void *aHit;
    aHit = (void *) LCHitAllocator.MallocSingle();
    return aHit;
}

inline void LCHit::operator delete(void *aHit)
{
    LCHitAllocator.FreeSingle((LCHit*)aHit);
}
#endif /* LCHIT_HH_ */
