#ifndef LCPrimaryGeneratorAction_hh
#define LCPrimaryGeneratorAction_hh 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4GeneralParticleSource.hh>
#include <G4Event.hh>

class LCPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction{
    public:
        LCPrimaryGeneratorAction(void);
        ~LCPrimaryGeneratorAction(void);
        void GeneratePrimaries(G4Event *evt);
    private:
        G4GeneralParticleSource *gps;
};

#endif
