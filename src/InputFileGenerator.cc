// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id: InputFileGenerator.cc,v 1.4 2007/08/31 16:02:45 mora Exp $
// $Name: mokka-06-07-patch02 $

#include "InputFileGenerator.hh"
#include "Setup.hh"

#include "G4ios.hh"
#include "HEPEvtInterface.hh"
#include "G4Event.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4UIcommand.hh"
#include "Randomize.hh"

#include <fstream>

InputFileGenerator::InputFileGenerator(G4String fileName): 
		VPrimaryGenerator(fileName)
{ 
  fGenerator = 0;
  SetupFileGenerator(fileName);
  Setup::Particle_Generator = fileName;
}

void InputFileGenerator::SetupFileGenerator(G4String generatorName)
{
    if (fGenerator) delete fGenerator; // dispose of the old generator

    G4VPrimaryGenerator *newGenerator = 0;

    // does the file exist?
    std::ifstream generatorFile(generatorName); // simply try and open it!
    if (!generatorFile) {
      G4cout << "Could not open \"" << generatorName << "\", please choose another file." << G4endl;
      return; // nothing has really happened yet, we can still exit here
    } else {
	generatorFile.close(); // one of the interfaces will open it again
    }

    // now we know the file exists, but what kind of file is it?
    const size_t dotPosition = generatorName.rfind("."); // find the last dot (suffix separator)
    const G4String nameSuffix = (dotPosition != std::string::npos) ? // found a dot?
      generatorName(dotPosition, generatorName.length() - dotPosition) : // yes: get suffix
      G4String(); // no: don't get anything



    if(!newGenerator)
      {
	if (nameSuffix == ".HEPEvt") newGenerator = new HEPEvtInterface(generatorName);
	/* ...insert any other interfaces here... */
	else { // filename has an unknown suffix (or no suffix at all)
	 G4Exception("InputFileGenerator:The generator has to be filename\n"
		     "with suffix \".HEPEvt\", \".stdhep\", or \".pairs\".",
		     nameSuffix,
		     RunMustBeAborted,
		     " unknown suffix !");
	}
      }
    // now we know that the new generator is really valid
  fGenerator = newGenerator; 
  SetGeneratorName(generatorName);
  // skip events in the generator file
  if ( Setup::EventStartNumber > 0 ) {
    G4cout << "Skipping first "<< Setup::EventStartNumber << " events..."<< G4endl;
    for ( int i=0 ; i<Setup::EventStartNumber; i++){
	G4Event *evt = new G4Event(); // just a dummy
	fGenerator->GeneratePrimaryVertex(evt);
	delete evt;
    }
  }

}

InputFileGenerator::~InputFileGenerator(void)
{
  if (fGenerator) delete fGenerator;
}

void InputFileGenerator::GeneratePrimaryVertex(G4Event *evt)
{
  if (fGenerator)
    fGenerator->GeneratePrimaryVertex(evt); // read event from a file
}

void InputFileGenerator::PrintGeneratorInfo(void)
{
  G4cout << "InputFileGenerator: generator file name is: " << GetGeneratorName()
		 << G4endl;
  // Additional info to be added
}
