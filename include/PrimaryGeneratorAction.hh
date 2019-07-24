#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction{
    public:
        PrimaryGeneratorAction(void);
        ~PrimaryGeneratorAction(void);
        void GeneratePrimaries(G4Event *evt);
    private:
        G4ParticleGun *fParticleGun;
};

#endif
