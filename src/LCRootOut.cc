/*
 * LCRootOut.cc
 *
 *  Created on: Apr 23, 2009
 *      Author: aguilar
 */

#include "LCRootOut.hh"
#include "Setup.hh"
#include "LCDetectorConstruction.hh"

#include "G4SystemOfUnits.hh"

TFile *LCRootOut::pRootFile = NULL;

LCRootOut::LCRootOut(){_file = NULL;}

LCRootOut::LCRootOut(const G4String name){
    RootOutFile = name;
    _file = NULL;
    _LcalData = NULL;
}

LCRootOut::~LCRootOut(){G4cout<<"LCRootOut is deleted!"<<G4endl;}

void LCRootOut::CreateNewTree(){
    // Create tree
    _LcalData = new TTree("Lcal", "Root file for LC hit data");

    // Create branches
    _LcalData->Branch("vX", &vX, "vX/D");
    _LcalData->Branch("vY", &vY, "vY/D");
    _LcalData->Branch("vZ", &vZ, "vZ/D");
    
    // Primary particles 
    _LcalData->Branch("numPrim", &numPrim, "numPrim/I"); // number of primary particles
    _LcalData->Branch("numHits", &numHits, "numHits/I"); // number of calo hits
    _LcalData->Branch("Etot", Etot, "Etot[2]/D"); // total energy deposit per side 1
    _LcalData->Branch("Emax", &Emax, "Emax/D"); // max hit energy

    // Tracks
    _LcalData->Branch("Tracks", "std::vector<Track_t>", &pTracks); // primary particles momenta

    // Hits
    _LcalData->Branch("Hits","std::vector<Hit_t>", &pHits); // calo hits
}

void LCRootOut::SetAddresses(){
    _LcalData->SetBranchAddress("vX", &vX); 
    _LcalData->SetBranchAddress("vY", &vY); 
    _LcalData->SetBranchAddress("vZ", &vZ); 
    _LcalData->SetBranchAddress("numPrim", &numPrim);
    _LcalData->SetBranchAddress("numHits", &numHits);
    _LcalData->SetBranchAddress("Etot", Etot);
    _LcalData->SetBranchAddress("Emax", &Emax);
    _LcalData->SetBranchAddress("Tracks", &pTracks); 
    _LcalData->SetBranchAddress("Hits",&pHits);
}

void LCRootOut::Init(){
    numHits = 0;
    numPrim = 0;
    Etot[0] = 0.;
    Etot[1] = 0.;
    Emax = 0.;
    pTracks = &Tracks;
    pHits = &Hits;

    // Open root file 
    if(!_file){
        G4cout<<"LCRootOut::Opening file: "<<Setup::RootFileName<<" write mode: "<<Setup::RootFileMode<<G4endl;
    
        _file = new TFile(Setup::RootFileName, Setup::RootFileMode);
        LCRootOut::pRootFile = _file;
        _LcalData = (TTree*)_file->Get("Lcal");

        // The following is attempt to fix weird GEANT4 behaviour:
        // TFile object does not prevent against overwriting exiting file
        // -in mode "NEW" or "CREATE" issues only warning message and continues
        // this results in "empty run". Results are not written to a file
        // -"RECREATE" mode causes overwriting exiting file without warning

        if(_file && Setup::RootFileMode == "UPDATE"){
            // This is correct situation. Don't create new tree, use existing one and set branch addresses.
            if(_LcalData) SetAddresses();
            else{
                G4cout<<"File: "<<Setup::RootFileName<<" opened "<<G4endl;
                CreateNewTree();
            }
        }
        else if(_file->IsZombie() && (Setup::RootFileMode == "NEW" || Setup::RootFileMode == "CREATE")){
            // Something is wrong - file exists, ROOT issues error message but run continues.
            // User is going to waste time runing job with no output saved in root file.
            // Abort the run
            G4Exception("LCRootOut::Init:Attempt to override file:", Setup::RootFileName, RunMustBeAborted, ". Aborting!");
        }
        else if(_file && Setup::RootFileMode == "RECREATE"){ 
            G4cerr<<"File: "<<Setup::RootFileName<<" is being overriden!!!"<<G4endl;
            CreateNewTree();
        }    
        else{
            // this covers "NEW"/"CREATE" (file not existing) situations
            G4cout<<"New empty file: "<<Setup::RootFileName<<" opened "<<G4endl;
            CreateNewTree();
        }
        
        G4cout<<"LCRootOut::Init completed."<<G4endl;
    }
    else{
        _LcalData = (TTree*)_file->Get("Lcal");
        if(!_LcalData) G4Exception("File ", Setup::RootFileName, RunMustBeAborted," does not have class <Lcal>");
    }
}

void LCRootOut::ProcessEvent(const G4Event* event, LCHitsCollection *collection){
    numHits = 0;
    numPrim = 0;
    Etot[0] = 0.;
    Etot[1] = 0.;
    Emax = 0.;

    // Get all primary MC particles
    G4int nv = event->GetNumberOfPrimaryVertex();
    G4int k=0;
    for(int v=0; v<nv; v++){
        G4PrimaryVertex *pv = event->GetPrimaryVertex(v);
        vX = pv->GetX0();
        vY = pv->GetY0();
        vZ = pv->GetZ0();
        G4PrimaryParticle *pp = pv->GetPrimary();
        while(pp){
            Track_t t;
            t.pX = (pp->GetMomentum()).getX();
            t.pY = (pp->GetMomentum()).getY();
            t.pZ = (pp->GetMomentum()).getZ();
            t.ID = pp->GetTrackID();
            t.PDG = pp->GetPDGcode();

            pTracks->push_back(t);
            k++;
            pp = pp->GetNext();
        }
    }

    numPrim = k;

    if(collection){
        numHits = collection->entries();
        G4int i = 0;
        while(i<numHits){
            Hit_t hit;
            hit.cellID = (*collection)[i]->GetCellCode();
            G4int side = (hit.cellID >> 24) & 0xff ;

            hit.eHit = (*collection)[i]->GetEnergy();
            hit.xCell = (hit.cellID>>8) & 0xFF;
            hit.yCell = (hit.cellID) & 0xFF;
            hit.zCell = (hit.cellID>>16) & 0xFF;
      
            hit.xHit = (*collection)[i]->GetXhit();
            hit.yHit = (*collection)[i]->GetYhit();
            hit.zHit = (*collection)[i]->GetZhit();
            hit.TOF = (*collection)[i]->GetTOF();
            bool cellInTestBeam = false;
            if(((hit.xCell == 12) && (hit.yCell > 49)) || ((hit.xCell == 13) && (hit.yCell > 45))) cellInTestBeam = true;
            if((Setup::Lcal_n_layers > 12) || ((Setup::Lcal_n_layers <= 12) && (cellInTestBeam))){
                Etot[side-1] += hit.eHit;
                if(Emax < hit.eHit) Emax = hit.eHit;
                pHits->push_back(hit);
            }
                i++;
        }

        _LcalData->Fill();
        // Clear vectors
        pTracks->clear();
        pHits->clear();
    }
}

void LCRootOut::End(){
    // Fill tree, do not close the file. It will be closed by main()
    _file->Write();
    _file->Close();
    G4cout<<"LCRootOut::Closed file: "<<Setup::RootFileName<<G4endl;
    delete _file;
    _file = NULL;
    _LcalData = NULL;
}
