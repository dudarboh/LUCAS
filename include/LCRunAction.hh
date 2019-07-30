#ifndef LCRUNACTION_HH_
#define LCRUNACTION_HH_ 1

#include "LCRootOut.hh"
#include "G4UserRunAction.hh"

class G4Run;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction(LCRootOut *RO);
        LCRunAction();
        ~LCRunAction();

        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction (const G4Run*);

        LCRootOut *fRootOut;
};

#endif
