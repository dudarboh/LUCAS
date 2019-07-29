/*
 * LCEventAction.hh
 * 2ModLumiCal
 *
 *  Created on: Apr 4, 2009
 *      Author: Jonathan Aguilar and Bogdan Pawlik
 *
 *      Modeled after Geant4 extended example RE01 event action class.
 *      Use this class to write data files
 */

#ifndef LCEVENTACTION_HH_
#define LCEVENTACTION_HH_ 1

#include "G4UserEventAction.hh"

#include "LCHit.hh"
#include "LCRootOut.hh"

#include "globals.hh"


class LCEventAction : public G4UserEventAction{
    public:
        LCEventAction(LCRootOut *RO);
        LCEventAction();
        virtual ~LCEventAction();

        virtual void BeginOfEventAction(const G4Event*);
        virtual void EndOfEventAction(const G4Event*);

    private:
        LCHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;

        G4int fHCID; // ID number for a collection of calorimeter hits
 
        LCRootOut *fRootOut; // Handles writing the ROOT tree
};

#endif