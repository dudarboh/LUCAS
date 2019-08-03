#include "LCPrimaryGeneratorAction.hh"

#include "G4PrimaryVertex.hh"

LCPrimaryGeneratorAction::LCPrimaryGeneratorAction(void){gps = new G4GeneralParticleSource();}

LCPrimaryGeneratorAction::~LCPrimaryGeneratorAction(void){delete gps;}

void LCPrimaryGeneratorAction::GeneratePrimaries(G4Event *evt){gps->GeneratePrimaryVertex(evt);}
