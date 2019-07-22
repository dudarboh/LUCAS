// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id: PrimaryGeneratorAction.cc,v 1.17 2008/07/03 14:35:26 musat Exp $
// $Name: mokka-06-07-patch02 $

#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"
#include "Setup.hh"

#include "ParticleGunGenerator.hh"
#include "InputFileGenerator.hh"

#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"

#include "G4SystemOfUnits.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(void)
{
  fMessenger = new PrimaryGeneratorMessenger(this);
  fPrimaryGenerator = new ParticleGunGenerator();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction(void)
{
  if (fPrimaryGenerator) delete fPrimaryGenerator;
  delete fMessenger;
}

void PrimaryGeneratorAction::SetGeneratorWithName(G4String generatorName)
{
  if (fPrimaryGenerator) // First dispose of the old generator 
	delete fPrimaryGenerator; 

  VPrimaryGenerator *newGenerator = 0;

  if(generatorName == "particleGun") 
	newGenerator = new ParticleGunGenerator();
  else {// Generator from input file 
	newGenerator = new InputFileGenerator(generatorName);

  }
  fPrimaryGenerator = newGenerator;
  Setup::Particle_Generator = generatorName;
 
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *evt)
{
  if (fPrimaryGenerator)
    fPrimaryGenerator->GeneratePrimaryVertex(evt); 
  ApplyLorentzTransformation(evt);
}

void PrimaryGeneratorAction::ApplyLorentzTransformation(G4Event *evt)
{
  const G4double alpha = Setup::lorentzTransAngle;
  if (alpha == 0.) return; // nothing to do

  // parameters of the Lorentz transformation matrix
  const G4double gamma = sqrt(1 + sqr(tan(alpha)));
  const G4double betagamma = tan(alpha);

  // scan through all vertices and all valid primary particles
  for (int iVertex = 0; iVertex < evt->GetNumberOfPrimaryVertex(); iVertex++) {
    G4PrimaryVertex *vertex = evt->GetPrimaryVertex(iVertex);
    for (int iParticle = 0; iParticle < vertex->GetNumberOfParticle(); iParticle++) {
      G4PrimaryParticle *particle = vertex->GetPrimary(iParticle);
      // does the particle have a valid particle definition attached?
      if (particle->GetG4code()) {

        // before the transformation
        const G4double px = particle->GetPx();
        const G4double py = particle->GetPy();
        const G4double pz = particle->GetPz();
	const G4double m  = particle->GetMass() ;

        // after the transformation (boost in x-direction)
        const G4double pxPrime = betagamma * sqrt(sqr(px) + sqr(py) + sqr(pz) + sqr(m)) + gamma * px;

        // py and pz remain the same, E changes implicitly with px
        particle->SetMomentum(pxPrime, py, pz);
      }
    }
    // the position of the vertex is not transformed here

  }
}
