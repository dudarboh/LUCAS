#ifndef LCActionInitialization_h
#define LCActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

class LCActionInitialization : public G4VUserActionInitialization{
    public:
        LCActionInitialization(G4String OutputFileName);
        virtual ~LCActionInitialization();
        virtual void BuildForMaster() const;
        virtual void Build() const;
        G4String fOutputFileName;
};

#endif
