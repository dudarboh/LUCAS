#include "LCRootOut.hh"

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
    _LcalData->Branch("Emax", &Emax, "Emax/D"); // max hit energy

    // Tracks
    _LcalData->Branch("Track_N", &Track_N, "Track_N/I");
    _LcalData->Branch("Track_px", Track_px, "Track_px[Track_N]/D");
    _LcalData->Branch("Track_py", Track_py, "Track_py[Track_N]/D");
    _LcalData->Branch("Track_pz", Track_pz, "Track_pz[Track_N]/D");
    _LcalData->Branch("Track_ID", Track_ID, "Track_ID[Track_N]/I");
    _LcalData->Branch("Track_PDG", Track_PDG, "Track_PDG[Track_N]/I");

    // Hits
    _LcalData->Branch("Hit_N",&Hit_N, "Hit_N/I");
    _LcalData->Branch("Hit_cellID", Hit_cellID, "Hit_cellID[Hit_N]/I");
    _LcalData->Branch("Hit_energy", Hit_energy, "Hit_energy[Hit_N]/D");
    _LcalData->Branch("Hit_xCell", Hit_xCell, "Hit_xCell[Hit_N]/D");
    _LcalData->Branch("Hit_yCell", Hit_yCell, "Hit_yCell[Hit_N]/D");
    _LcalData->Branch("Hit_zCell", Hit_zCell, "Hit_zCell[Hit_N]/D");
    _LcalData->Branch("Hit_xHit", Hit_xHit, "Hit_xHit[Hit_N]/D");
    _LcalData->Branch("Hit_yHit", Hit_yHit, "Hit_yHit[Hit_N]/D");
    _LcalData->Branch("Hit_zHit", Hit_zHit, "Hit_zHit[Hit_N]/D");
    _LcalData->Branch("Hit_TOF", Hit_TOF, "Hit_TOF[Hit_N]/D");

}

void LCRootOut::SetAddresses(){
    _LcalData->SetBranchAddress("vX", &vX); 
    _LcalData->SetBranchAddress("vY", &vY); 
    _LcalData->SetBranchAddress("vZ", &vZ); 
    _LcalData->SetBranchAddress("Emax", &Emax);

    _LcalData->SetBranchAddress("Track_N", &Track_N);
    _LcalData->SetBranchAddress("Track_px", Track_px);
    _LcalData->SetBranchAddress("Track_py", Track_py);
    _LcalData->SetBranchAddress("Track_pz", Track_pz);
    _LcalData->SetBranchAddress("Track_ID", Track_ID);
    _LcalData->SetBranchAddress("Track_PDG", Track_PDG);

    _LcalData->SetBranchAddress("Hit_N", &Hit_N);
    _LcalData->SetBranchAddress("Hit_cellID", Hit_cellID);
    _LcalData->SetBranchAddress("Hit_energy", Hit_energy);
    _LcalData->SetBranchAddress("Hit_xCell", Hit_xCell);
    _LcalData->SetBranchAddress("Hit_yCell", Hit_yCell);
    _LcalData->SetBranchAddress("Hit_zCell", Hit_zCell);
    _LcalData->SetBranchAddress("Hit_xHit", Hit_xHit);
    _LcalData->SetBranchAddress("Hit_yHit", Hit_yHit);
    _LcalData->SetBranchAddress("Hit_zHit", Hit_zHit);
    _LcalData->SetBranchAddress("Hit_TOF", Hit_TOF);
}

void LCRootOut::Init(){

    G4String filename = "Lucas_output.root";
    G4String open_option = "CREATE";
    // Open root file 
    if(!_file){
        _file = new TFile(filename, open_option);
        LCRootOut::pRootFile = _file;
        _LcalData = (TTree*)_file->Get("Lcal");

        CreateNewTree();
        
        G4cout<<"LCRootOut::Init completed."<<G4endl;
    }
    else{
        _LcalData = (TTree*)_file->Get("Lcal");
        if(!_LcalData) G4Exception("File ", filename, RunMustBeAborted," does not have class <Lcal>");
    }
}

void LCRootOut::ProcessEvent(const G4Event* event, LCHitsCollection *collection){
    Track_N = 0;
    Hit_N = 0;
    Emax = 0.;

    G4int nVertices = event->GetNumberOfPrimaryVertex();
    G4int k=0;
    for(int v=0; v<nVertices; v++){
        G4PrimaryVertex *pVertex = event->GetPrimaryVertex(v);
        vX = pVertex->GetX0();
        vY = pVertex->GetY0();
        vZ = pVertex->GetZ0();
        G4PrimaryParticle *pParticle = pVertex->GetPrimary();
        while(pParticle){
            Track_px[k] = (pParticle->GetMomentum()).getX();
            Track_py[k] = (pParticle->GetMomentum()).getY();
            Track_pz[k] = (pParticle->GetMomentum()).getZ();
            Track_ID[k] = pParticle->GetTrackID();
            Track_PDG[k] = pParticle->GetPDGcode();
            k++;
            pParticle = pParticle->GetNext();
        }
    }

    Track_N = k;
    if(Track_N > 999)G4Exception("Number of tracks ", "Track_N", RunMustBeAborted," exceed array size of 999!");

    if(collection){
        G4cout<<"I WAS HEREEE"<<endl;
        Hit_N = collection->entries();
        G4int i = 0;
        if(Hit_N > 2500)G4Exception("Number of hits ", "Hit_N", RunMustBeAborted," exceed array size of 2500!");
        while(i<Hit_N){
            Hit_cellID[i] = (*collection)[i]->GetCellCode();

            Hit_energy[i] = (*collection)[i]->GetEnergy();
            Hit_xCell[i] = (Hit_cellID[i]>>8) & 0xFF;
            Hit_yCell[i] = (Hit_cellID[i]) & 0xFF;
            Hit_zCell[i] = (Hit_cellID[i]>>16) & 0xFF;
      
            Hit_xHit[i] = (*collection)[i]->GetXhit();
            Hit_yHit[i] = (*collection)[i]->GetYhit();
            Hit_zHit[i] = (*collection)[i]->GetZhit();

            if(Emax < Hit_energy[i]) Emax = Hit_energy[i];
            i++;
        }

        _LcalData->Fill();

    }
}

void LCRootOut::End(){
    // Fill tree, do not close the file. It will be closed by main()
    _file->Write();
    _file->Close();
    G4cout<<"LCRootOut::Closed file: "<<G4endl;
    delete _file;
    _file = NULL;
    _LcalData = NULL;
}
