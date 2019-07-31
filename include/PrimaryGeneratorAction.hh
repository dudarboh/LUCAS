#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh 1

#include <G4GeneralParticleSource.hh>
#include <G4Event.hh>
#include "G4VUserPrimaryGeneratorAction.hh"

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction{
    public:
        PrimaryGeneratorAction(void);
        ~PrimaryGeneratorAction(void);
        void GeneratePrimaries(G4Event *evt);
    private:
        G4GeneralParticleSource *gps;
};

#endif
