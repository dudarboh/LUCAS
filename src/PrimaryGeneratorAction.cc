
#include "PrimaryGeneratorAction.hh"
#include <G4ParticleDefinition.hh>
#include <G4Electron.hh>
#include "G4PrimaryVertex.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction(void){gps = new G4GeneralParticleSource();}

PrimaryGeneratorAction::~PrimaryGeneratorAction(void){delete gps;}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *evt){gps->GeneratePrimaryVertex(evt);}
