#ifndef LCActionInitialization_h
#define LCActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class LCActionInitialization : public G4VUserActionInitialization{
    public:
        LCActionInitialization();
        virtual ~LCActionInitialization();
        virtual void BuildForMaster() const;
        virtual void Build() const;
};

#endif