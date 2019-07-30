#include "LCRootOut.hh"

LCRootOut::LCRootOut(){_file = NULL;}

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

    // Hits
    _LcalData->Branch("Hit_N",&Hit_N, "Hit_N/I");
    _LcalData->Branch("Hit_energy", Hit_energy, "Hit_energy[Hit_N]/D");
    _LcalData->Branch("Hit_xCell", Hit_xCell, "Hit_xCell[Hit_N]/I");
    _LcalData->Branch("Hit_yCell", Hit_yCell, "Hit_yCell[Hit_N]/I");
    _LcalData->Branch("Hit_zCell", Hit_zCell, "Hit_zCell[Hit_N]/I");
    _LcalData->Branch("Hit_xHit", Hit_xHit, "Hit_xHit[Hit_N]/D");
    _LcalData->Branch("Hit_yHit", Hit_yHit, "Hit_yHit[Hit_N]/D");
    _LcalData->Branch("Hit_zHit", Hit_zHit, "Hit_zHit[Hit_N]/D");

}

void LCRootOut::Init(){

    G4String filename = "Lucas_output.root";
    G4String open_option = "RECREATE";
    // Open root file 
    if(!_file){
        _file = new TFile(filename, open_option);

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
            k++;
            pParticle = pParticle->GetNext();
        }
    }

    Track_N = k;
    if(Track_N > 999)G4Exception("Number of tracks ", "Track_N", RunMustBeAborted," exceed array size of 999!");

    if(collection){
        G4cout<<"Collection events:"<< collection->entries()<<G4endl;
        Hit_N = collection->entries();
        G4int i = 0;
        if(Hit_N > 2500)G4Exception("Number of hits ", "Hit_N", RunMustBeAborted," exceed array size of 2500!");
        while(i<Hit_N){

            Hit_energy[i] = (*collection)[i]->GetEnergy();
            Hit_xCell[i] = (*collection)[i]->GetXcell();
            Hit_yCell[i] = (*collection)[i]->GetYcell();
            Hit_zCell[i] = (*collection)[i]->GetZcell();

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
