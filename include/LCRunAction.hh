#ifndef LCRunAction_h
#define LCRunAction_h 1

#include "G4UserRunAction.hh"
#include "LCPrimaryGeneratorAction.hh"

class G4Run;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction();
        virtual ~LCRunAction();

        G4Run* GenerateRun();
        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction (const G4Run*);
};

#endif
