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

LCRootOut::LCRootOut()
{ _file   = NULL;
}

LCRootOut::LCRootOut(const G4String name )
{
  RootOutFile = name;
  _file = NULL;
  _LcalData = NULL;
}

LCRootOut::~LCRootOut()
{ 
  G4cout << " LCRootOut deleted ! " << G4endl;
}

void LCRootOut::CreateNewTree()
{
    // create tree
    //
  _LcalData = new TTree("Lcal", "Root file for LC hit data");
    //
    // setup dictionary
    //
    //G__cpp_setup_globalLucasDict();
    // 

    // create branches
    //
    // vertex pos 
    _LcalData->Branch("vX", &vX, "vX/D");       // Vertex primary particle
    _LcalData->Branch("vY", &vY, "vY/D");       // Vertex primary particle
    _LcalData->Branch("vZ", &vZ, "vZ/D");       // Vertex primary particle
    // primary particles 
    _LcalData->Branch("numPrim", &numPrim, "numPrim/I");               // number of primary particles
    _LcalData->Branch("numHits", &numHits, "numHits/I");               // number of calo hits
    _LcalData->Branch("Etot", Etot, "Etot[2]/D");                      // total  energy deposit per side 1
    _LcalData->Branch("Emax", &Emax, "Emax/D");                        //  max hit energy
    // tracks
    _LcalData->Branch("Tracks","std::vector< Track_t >", &pTracks );   // primary particles momenta
    // hits
    _LcalData->Branch("Hits","std::vector< Hit_t >", &pHits );         // calo hits
}

void LCRootOut::SetAddresses()
{
      _LcalData->SetBranchAddress("vX", &vX ); 
      _LcalData->SetBranchAddress("vY", &vY ); 
      _LcalData->SetBranchAddress("vZ", &vZ ); 
      _LcalData->SetBranchAddress("numPrim", &numPrim );
      _LcalData->SetBranchAddress("numHits", &numHits );
      _LcalData->SetBranchAddress("Etot", Etot );
      _LcalData->SetBranchAddress("Emax", &Emax );
      _LcalData->SetBranchAddress("Tracks", &pTracks ); 
      _LcalData->SetBranchAddress("Hits",&pHits );
}

void LCRootOut::Init()
{
  // 
    numHits = 0; 
    numPrim = 0;
    Etot[0] = 0. ;
    Etot[1] = 0. ;
    Emax  = 0. ;
    pTracks = &Tracks;
    pHits   = &Hits;

  // open root file 
  //
  if( !_file )  
    {
      G4cout << "LCRootOut:: Opening file: " << Setup::RootFileName 
	     << " write mode : "<< Setup::RootFileMode << G4endl;
    
    _file = new TFile( Setup::RootFileName, Setup::RootFileMode);
    LCRootOut::pRootFile = _file;

    _LcalData = (TTree*)_file->Get("Lcal");

   // the following is attempt to fix weird GEANT4 behaviour :
    // TFile object does not prevent against overwriting exiting file 
    // - in mode "NEW" or "CREATE" issues only warning message and continues
    //   this results in "empty run" results are not written to a file
    // - "RECREATE" mode causes overwriting exiting file without warning
    // 

    if( _file && Setup::RootFileMode == "UPDATE" )
      {
	if ( _LcalData ) {
      // this is correct situation
      // don't create new tree, use existing one
      // and set branch addresses
      //
      SetAddresses();
	}else{
	G4cout << " File : " << Setup::RootFileName << " opened " << G4endl;
	CreateNewTree();
	}

     
      }
    else if ( (_file->IsZombie()) && ( Setup::RootFileMode == "NEW" || Setup::RootFileMode == "CREATE" ))
      {
      
        // something is wrong - file exists, ROOT issues error message but run continues, 
        // user is going to waste time runing job with no output saved in
        // root file.
	// Abort the run
	G4Exception ( " LCRootOut::Init: Attempt to override file :",
		      Setup::RootFileName,
		      RunMustBeAborted, ". Aborting !");

      }
    else if (  _file && Setup::RootFileMode == "RECREATE" )
      { 
	// G4Exception ( " Attempt to override existing file :" + Setup::RootFileName + ". Aborting !");
	G4cerr << " File : "<< Setup::RootFileName << " is being overriden !!!!!!!! " << G4endl;
	CreateNewTree();
      }    
    else
      {
	// this covers following situations:
	//         "NEW"/"CREATE"    (file not existing)
	// new file was opened
	//
	G4cout << " New empty file : " << Setup::RootFileName << " opened " << G4endl;
	CreateNewTree();
      }
    G4cout << "LCRootOut::Init completed." << G4endl;
    }else{
     _LcalData = (TTree*)_file->Get("Lcal");
     if ( !_LcalData ) 
       G4Exception( " File ",
		    Setup::RootFileName,
		    RunMustBeAborted, " does not have class <Lcal>");
  } // if(!_file)
}

void LCRootOut::ProcessEvent(const G4Event* event, LCHitsCollection *collection)
{
    numHits = 0; 
    numPrim = 0;
    Etot[0] = 0. ;
    Etot[1] = 0. ;
    Emax  = 0. ;
    //
    // get all primary MC particles
    G4int nv = event->GetNumberOfPrimaryVertex();
    G4int k=0;
    for (int v = 0; v < nv ; v++) {
        G4PrimaryVertex *pv = event->GetPrimaryVertex(v);
	vX = pv->GetX0();
	vY = pv->GetY0();
	vZ = pv->GetZ0();
	G4PrimaryParticle *pp = pv->GetPrimary();
        while (pp) {
	 
	  Track_t t;
	  t.pX = (pp->GetMomentum()).getX();
	  t.pY = (pp->GetMomentum()).getY();
	  t.pZ = (pp->GetMomentum()).getZ();
	  t.ID =  pp->GetTrackID();
	  t.PDG = pp->GetPDGcode();
	  //
	  pTracks->push_back( t );
	  k++;
	  pp = pp->GetNext();
        }
    }
    numPrim = k;
    //    G4cout<<" Number of primary particles: "<<numPrim << G4endl;

    if( collection ){
    numHits = collection->entries();
    //
    G4int i = 0;
    while ( i < numHits){
      Hit_t hit;
      hit.cellID =(*collection)[i]->GetCellCode();
      G4int    side = (hit.cellID >> 24) & 0xff ;

      hit.eHit  = (*collection)[i]->GetEnergy();
      
      
      //hit.xCell = (*collection)[i]->GetXcell();
      //hit.yCell = (*collection)[i]->GetYcell();
      //hit.zCell = (*collection)[i]->GetZcell();
      hit.xCell = (hit.cellID>>8) & 0xFF;
      hit.yCell = (hit.cellID) & 0xFF;
      hit.zCell = (hit.cellID>>16) & 0xFF;
      
      hit.xHit  = (*collection)[i]->GetXhit();
      hit.yHit  = (*collection)[i]->GetYhit();
      hit.zHit  = (*collection)[i]->GetZhit();
      hit.TOF   = (*collection)[i]->GetTOF();
      bool cellInTestBeam = false; // to mask 2014 TB
      if ( ((hit.xCell == 12)&&(hit.yCell>49))  ||  ((hit.xCell == 13)&&(hit.yCell>45)) )
	cellInTestBeam=true;
      if((Setup::Lcal_n_layers > 12) ||( (Setup::Lcal_n_layers <= 12)&&(cellInTestBeam) ) ){
		Etot[side-1] += hit.eHit ;
		if ( Emax < hit.eHit  ) Emax = hit.eHit;
		pHits->push_back( hit );
      }
      //
      i++;
    }

    _LcalData->Fill();
    //
    // clear vectors
    // 
    pTracks->clear();
    pHits->clear();
    }


    /*    G4cout << "LCRootOut::ProcessEvent completed." << G4endl;
    G4cout << " Number of Hits "<< numHits << G4endl;
    G4cout << " Energy " << Etot1  / GeV << " [GeV]"<< G4endl; */
}

void LCRootOut::ProcEventAccumulate( LCHitsCollection *collection )
{
  // - B.P. 29.05.2009
  // this method is devoted to merge events from entire run
  // into one tree. It is to be used solely for beam-background data.
  // Primaries are not stored in the tree ( typically there is 10^5
  // primaries per event ). The list of tracks contributing to
  // a hit is neglected as well.  

    if( collection ){

    G4int       i = 0;
    G4int   nHits = collection->entries();
 
    while ( i < nHits){

        G4int address = (*collection)[i]->GetCellCode();
	G4int    side = (address >> 24) & 0xff ;
	G4double eH  = (*collection)[i]->GetEnergy();
	unsigned int it = 0;
	while ( it < theUsedCells.size() ){
	  if( theUsedCells[it] == address ) break;
	  it++;
	}

	if ( it == theUsedCells.size() ) { // cellID not in the list, add

	  theUsedCells.push_back( address );
	  Hit_t hit;
	  hit.eHit  = eH;
	  hit.xCell = (*collection)[i]->GetXcell();
	  hit.yCell = (*collection)[i]->GetYcell();
	  hit.zCell = (*collection)[i]->GetZcell();
	  hit.xHit  = (*collection)[i]->GetXhit();
	  hit.yHit  = (*collection)[i]->GetYhit();
	  hit.zHit  = (*collection)[i]->GetZhit();
	  hit.TOF   = (*collection)[i]->GetTOF();
	  if ( Emax < eH ) Emax = eH;
	  pHits->push_back( hit ); 
	  numHits++;
	}else{        // cell in list, increase energy deposit only
	  (*pHits)[it].eHit += eH;
	  if ( (*pHits)[it].eHit > Emax ) Emax = (*pHits)[it].eHit;
	}
        Etot[side-1] += eH ;
        i++;
    }

    }

}

void LCRootOut::End()
{
  // end of run action
  if ( Setup::AccumulateEvents ){          //fill tree, do not close the file
    _LcalData->Fill();                     // it will be closed by main()
    theUsedCells.clear();
  }else{
    _file->Write();
    _file->Close();
    G4cout << "LCRootOut::Closed file: " << Setup::RootFileName << G4endl;
    delete _file;
    _file = NULL;
    _LcalData = NULL;
  }
}
