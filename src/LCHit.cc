/*
 * LCHit.cc
 * 2ModLumiCal
 *
 *  Created on: Mar 28, 2009
 *      Author: aguilar
 *
 */

#include "LCHit.hh"
//#include "LCPrimaryContribution.hh"

#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"

#include <assert.h>
#include <map>

// don't know if i need these - CalHit doesn't use them
#include "G4AttValue.hh"
#include "G4AttDef.hh"
#include "G4AttCheck.hh"

G4Allocator<LCHit> LCHitAllocator;

// constructor defined in the header


//destructor
LCHit::~LCHit()
{
    trackIDs->clear();
    delete trackIDs;
}

// Accumulates energy from primary particles that enter the cell
// This is the central function of LCHit
void LCHit::AddEdep(G4int pPID, G4int pPDG, G4double de)
{
    NoOfContributingHits ++;
    Energy += de; // increment energy deposition per hit
    PrimaryIDMapIterator iter = trackIDs->find(pPID);
    if (iter == trackIDs->end()) {
      trackIDs->insert( PrimaryIDPair(pPID, pPDG) );
    }
}


// Print, Draw hits

void LCHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if(pVVisManager) {
        PrimaryIDMap::iterator p = trackIDs -> begin();
        PrimaryIDMap::value_type x = *p;
        G4int PID = x.first;
        G4Colour colour(((PID%2)+1.)/2.,((PID%3)+1.)/3.,1.);
        //G4Colour colour(1.,1.,1.);
        G4VisAttributes attribs(colour);
        G4Circle circle;
        circle.SetPosition(G4Point3D(Xcell,Ycell,Zcell));
        circle.SetScreenDiameter (2.0);
        circle.SetFillStyle (G4Circle::filled);
        circle.SetVisAttributes(attribs);
        pVVisManager->Draw(circle);
    }
}
void LCHit::Print()
{  
   PrimaryIDMapIterator iter;
   PrimaryIDMapIterator ite0 = trackIDs->begin();
   if( ite0 != trackIDs->end() )
   G4cout << " Tracks contributed to hit : " << G4endl;
   for (iter=ite0 ; iter != trackIDs->end(); iter++)
     { 
       G4int PID = ( iter->first );
       G4int PDG = ( iter->second);
       G4cout<<"     ID: "<<PID<<"  PDG: "<<PDG<<G4endl;
     }

}

// Save a file, Load a File - from CalHit
void   LCHit::Save(FILE * ) { }
G4bool LCHit::Load(FILE * ) { return false; }

// eof
