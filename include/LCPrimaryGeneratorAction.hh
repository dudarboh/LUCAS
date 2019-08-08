#ifndef LCPrimaryGeneratorAction_h
#define LCPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4GeneralParticleSource;
class G4Event;

class LCPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction{
    public:
        LCPrimaryGeneratorAction();
        virtual ~LCPrimaryGeneratorAction();
        virtual void GeneratePrimaries(G4Event *evt);
    private:
        G4GeneralParticleSource *gps;
};

#endif
