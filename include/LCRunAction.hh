#ifndef LCRUNACTION_HH_
#define LCRUNACTION_HH_ 1

#include "G4UserRunAction.hh"

class G4Run;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction(G4bool rootOutput);
        LCRunAction();
        ~LCRunAction();

        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction (const G4Run*);

        G4bool fRootOutput;
};

#endif
