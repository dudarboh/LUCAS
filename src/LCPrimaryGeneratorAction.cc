#include "LCPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

//Simple GPS source.. All setup is done in Macro File
LCPrimaryGeneratorAction::LCPrimaryGeneratorAction(){gps = new G4GeneralParticleSource();}
LCPrimaryGeneratorAction::~LCPrimaryGeneratorAction(){delete gps;}
void LCPrimaryGeneratorAction::GeneratePrimaries(G4Event *evt){gps->GeneratePrimaryVertex(evt);}
