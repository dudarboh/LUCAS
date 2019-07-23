
#include "G4Types.hh"

#include "G4ios.hh"
#include "HEPEvtInterface.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4HEPEvtParticle.hh"
#include "G4ExceptionSeverity.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

HEPEvtInterface::HEPEvtInterface(char* evfile){
    inputFile.open(evfile);
    if(inputFile){fileName = evfile;}
    else{
        G4Exception("HEPEvtInterface:: cannot open file: ", evfile, RunMustBeAborted, " Run Aborted !");
    }
    G4ThreeVector zero;
    particle_position = zero;
    particle_time = 0.0;
}

HEPEvtInterface::HEPEvtInterface(G4String evfile){
    const char* fn = evfile.data();
    inputFile.open((char*)fn);
    if(inputFile){fileName = evfile;}
    else{
        G4Exception("HEPEvtInterface:: cannot open file: ", evfile, RunMustBeAborted, " Run Aborted !");
    }
    
    G4ThreeVector zero;
    particle_position = zero;
    particle_time = 0.0;
}

HEPEvtInterface::~HEPEvtInterface(){;}

void HEPEvtInterface::GeneratePrimaryVertex(G4Event* evt){
    G4int NHEP;  // number of entries
    inputFile>>NHEP;
    if(inputFile.eof()){
        // eof is not error. Force gently end of run
        G4cout<<" EOF encountered - end run"<<G4endl;
        G4RunManager::GetRunManager()->AbortRun(true);
        return;
    }

    for(G4int IHEP=0; IHEP<NHEP; IHEP++ ){
        G4int ISTHEP; // status code
        G4int IDHEP; // PDG code
        G4int JDAHEP1; // first daughter
        G4int JDAHEP2; // last daughter
        G4double PHEP1; // px in GeV
        G4double PHEP2; // py in GeV
        G4double PHEP3; // pz in GeV
        G4double PHEP5; // mass in GeV

        inputFile>>ISTHEP>>IDHEP>>JDAHEP1>>JDAHEP2>>PHEP1>>PHEP2>>PHEP3>>PHEP5;

        // create G4PrimaryParticle object
        G4PrimaryParticle *particle = new G4PrimaryParticle(IDHEP, PHEP1*GeV, PHEP2*GeV, PHEP3*GeV);
        particle->SetMass(PHEP5*GeV);

        // create HEPEvtParticle object
        G4HEPEvtParticle *hepParticle = new G4HEPEvtParticle(particle, ISTHEP, JDAHEP1, JDAHEP2);

        // Store
        HPlist.push_back(hepParticle);
    }

    // check if there is at least one particle
    if(HPlist.size() == 0) return; 

    // make connection between daughter particles decayed from the same mother
    for(size_t i=0; i<HPlist.size(); i++){
        // if it has daughters
        if(HPlist[i]->GetJDAHEP1()>0){
            G4int jda1 = HPlist[i]->GetJDAHEP1()-1; // FORTRAN index starts from 1
            G4int jda2 = HPlist[i]->GetJDAHEP2()-1; // but C++ starts from 0.
            G4PrimaryParticle *mother = HPlist[i]->GetTheParticle();
            for(G4int j=jda1; j<=jda2; j++){
                G4PrimaryParticle *daughter = HPlist[j]->GetTheParticle();
                if(HPlist[j]->GetISTHEP()>0){
                    mother->SetDaughter(daughter);
                    HPlist[j]->Done();
                }
            }
        }
    }

    // create G4PrimaryVertex object
    G4PrimaryVertex *vertex = new G4PrimaryVertex(particle_position, particle_time);

    // put initial particles to the vertex
    for(size_t ii=0; ii<HPlist.size(); ii++){
        // ISTHEP of daughters had been set to negative
        if(HPlist[ii]->GetISTHEP()>0){
            G4PrimaryParticle *initialParticle = HPlist[ii]->GetTheParticle();
            vertex->SetPrimary(initialParticle);
        }
    }

    // clear G4HEPEvtParticles
    for(size_t iii=0;iii<HPlist.size();iii++) delete HPlist[iii];
    HPlist.clear();

    // Put the vertex to G4Event object
    evt->AddPrimaryVertex(vertex);
}

