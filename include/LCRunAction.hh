/*
 * LCRunAction.hh
 * 2ModLumiCal
 *
 *  Created on: Apr 8, 2009
 *      Author: aguilar
 */

#ifndef LCRUNACTION_HH_
#define LCRUNACTION_HH_

#include "LCRootOut.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <iostream>
#include <fstream>


class G4Run;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction(LCRootOut *RO);
        LCRunAction();
        ~LCRunAction();

        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction (const G4Run*);
        void Print(G4String flag, const G4Run*);

        LCRootOut *RootOut;
};

#endif
