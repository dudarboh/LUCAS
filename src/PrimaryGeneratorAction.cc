#include "PrimaryGeneratorAction.hh"

#include <G4ParticleGun.hh>
#include <G4ParticleDefinition.hh>
#include <G4Electron.hh>

#include "G4PrimaryVertex.hh"

#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(void){
    fParticleGun = new G4ParticleGun(G4Electron::Electron());
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
    fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, 0));
    fParticleGun->SetParticleEnergy(5*GeV);}

PrimaryGeneratorAction::~PrimaryGeneratorAction(void){delete fParticleGun;}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *evt){fParticleGun->GeneratePrimaryVertex(evt);}
