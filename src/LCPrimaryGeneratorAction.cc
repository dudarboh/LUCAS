#include "LCPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

LCPrimaryGeneratorAction::LCPrimaryGeneratorAction(){gps = new G4GeneralParticleSource();}
LCPrimaryGeneratorAction::~LCPrimaryGeneratorAction(){delete gps;}
void LCPrimaryGeneratorAction::GeneratePrimaries(G4Event *evt){gps->GeneratePrimaryVertex(evt);}
