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

using namespace std;

InputFileGenerator::InputFileGenerator(G4String fileName):VPrimaryGenerator(fileName){ 
    fGenerator = 0;
    SetupFileGenerator(fileName);
    Setup::Particle_Generator = fileName;
}

void InputFileGenerator::SetupFileGenerator(G4String generatorName){
    //Delete old generator
    if(fGenerator) delete fGenerator;
    G4VPrimaryGenerator *newGenerator = 0;

    //Try open the file
    ifstream generatorFile(generatorName); 
    if(!generatorFile){
        G4cout<<"Could not open \""<<generatorName<<"\" , please choose another file."<<G4endl;
        return;
    }
    else{
        //One of the interfaces will open it again
        generatorFile.close();
    }

    // Check file type
    //Find last dot
    const size_t dotPosition = generatorName.rfind(".");
    
    // If found a dot get suffix, otherwise get nothing
    if(dotPosition != string::npos){
        const G4String nameSuffix = generatorName(dotPosition, generatorName.length() - dotPosition)
    }
    else{
        G4String();  
    }


    if(!newGenerator){
        if(nameSuffix == ".HEPEvt") newGenerator = new HEPEvtInterface(generatorName);
        /* ...insert any other interfaces here... */
        else{
            // filename has an unknown suffix (or no suffix at all)
            G4Exception("InputFileGenerator:The generator has to be filename\n"
            "with suffix \".HEPEvt\", \".stdhep\", or \".pairs\".",
            nameSuffix,
            RunMustBeAborted,
            " unknown suffix !");
        }
    }
    
    // Now we know that the new generator is really valid
    fGenerator = newGenerator; 
    SetGeneratorName(generatorName);
    
    // Skip events in the generator file
    if(Setup::EventStartNumber>0){
        G4cout << "Skipping first "<< Setup::EventStartNumber << " events..."<< G4endl;
        for(int i=0; i<Setup::EventStartNumber; i++){
            G4Event *evt = new G4Event(); // just a dummy
            fGenerator->GeneratePrimaryVertex(evt);
            delete evt;
        }
    }
}

InputFileGenerator::~InputFileGenerator(void){ if(fGenerator) delete fGenerator;}

void InputFileGenerator::GeneratePrimaryVertex(G4Event *evt){
    // Read event from a file
    if(fGenerator) fGenerator->GeneratePrimaryVertex(evt); 
}

void InputFileGenerator::PrintGeneratorInfo(void){
    G4cout<<"InputFileGenerator: generator file name is: "<<GetGeneratorName()<<G4endl;
    // Additional info to be added
}
