#include <time.h>

#include "LCEventAction.hh"
#include "G4SDManager.hh"
LCSensitiveDetector::LCSensitiveDetector(G4String sdname,
                                        G4double rmin,
                                        G4double phi0,
                                        G4double cRho,
                                        G4double cPhi,
                                        G4int nCellRho,
                                        G4int nCellPhi,
                                        G4bool cellvirtual)
    :G4VSensitiveDetector(sdname),
    fCollectionID(-1),
    fSDName(sdname),
    fHitsCollection(0),
    fVirtualCell(cellvirtual){

    collectionName.insert("SDHitsCollection"); // a G4VSensitiveDetector privat member

    hitMap = new LCHitMap;

    // Tell LCSensDet the geometric parameters of LumiCal
    fCalRhoMin = rmin;
    fCalPhiOffset = phi0;
    fCellDimRho = cRho;
    fCellDimPhi = cPhi;
    fNumCellsRho = nCellRho;
    fNumSectorsPhi = nCellPhi;

}

LCSensitiveDetector::~LCSensitiveDetector(){
    hitMap->clear();
    delete hitMap;
}

G4bool LCSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *ROhits){
    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();

    G4double edep = step->GetTotalEnergyDeposit();

    if(step->GetTrack()->GetParentID() <= 0){
        fPrimaryID = step->GetTrack()->GetTrackID();
        fPrimaryPDG = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    }

    G4ThreeVector GlobalHitPos = ((step->GetPreStepPoint()->GetPosition()) + (step->GetPostStepPoint()->GetPosition())) / 2.;

    G4int LC_num = 1, layer_num = 0, sector_num = 0, cell_num = 0;

    // Layer = copyNumber of mother (Slot) volume
    layer_num = touchable->GetHistory()->GetVolume(1)->GetCopyNo();
    //G4cout<<touchable->GetHistory()->GetVolume(1)->GetCopyNo()<<" "<<touchable->GetHistory()->GetVolume(1)->GetName()<<endl;
        
    if(fVirtualCell){
        G4ThreeVector LocalHitPos = touchable->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);
        G4double rho = LocalHitPos.getRho();
        G4double phi = LocalHitPos.getPhi();
        phi = (phi < 0.) ? phi + 2. * M_PI : phi; 
        cell_num = (G4int)floor(( rho - (fCalRhoMin-fCellDimRho/2.) ) / fCellDimRho );
        cell_num = (cell_num < 0) ? 0 : cell_num; 
        cell_num = (cell_num >fNumCellsRho) ? fNumCellsRho : cell_num; 
        sector_num = (G4int)floor(phi / fCellDimPhi) + 1;
    }

    // ENCODE THE HIT

    // Use bitwise operations to store the location of a cell in a single
    // 32-bit word
    // 4 bytes, 4 levels in the volume tree - 1 byte per volume
    // | LC number | Layer number | Sector number | Cell number |
    // note - this only works because no volume will have a number > 255 (2^8)
    // LumiCal only uses id0; id1 is a legacy from the Mokka version -- removed

    G4int cellID;
    cellID = 0;

    cellID |= (cell_num << 0); // store the cell # in the lowest 8 digits
    cellID |= (sector_num << 8); // shift the sector # to the next byte up
    cellID |= (layer_num << 16); // shift the layer # to the next byte up
    cellID |= (LC_num << 24); // shift the LumiCal # to the highest byte

    // get local and global position using touchable
    // check if the cell has been hit before, and check the indices
    if(!FindHit(cellID, edep)
        && !((cell_num > fNumCellsRho)
        || (sector_num > fNumSectorsPhi))){
        // Global hit coordinates
        // FG: the local coordinate system of the cell replica is the same as for the disk
        // containing the the pad - rotated by phi of the phi replica, i.e.
        // the origin is at the center of the disk
        // hence the position of the cell center in its coordinate system is given by:
        G4ThreeVector localCellPos(fCalRhoMin + ((G4double)cell_num) * fCellDimRho, 0., 0.);

        localCellPos.setPhi((0.5 + ((G4double)sector_num - 1)) * fCellDimPhi);

        // compute GlobalCellPos based on touchable with localCellPos
        G4ThreeVector GlobalCellPos = touchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(localCellPos);

        // assert id w/in valid range using bitwise ops
        // (0xff = 255 in hex)
        assert((cellID & 0xff) <= 120);

        // Use all this information to create an LCHit instance
        LCHit *theHit = new LCHit(GlobalHitPos.x(),
            GlobalHitPos.y(),
            GlobalHitPos.z(),
            GlobalCellPos.x(), // GlobalCellPosRho
            GlobalCellPos.y(), // GlobalCellPosPhi
            GlobalCellPos.z(), // GlobalCellPosZ
            edep,
            cellID);

        //Insert the hit and cellID into the hitmap
        hitMap->insert(LCHitMapPair(cellID, theHit));
    }
    return true;
}


void LCSensitiveDetector::Initialize(G4HCofThisEvent *HCE){
    if(fCollectionID < 0) fCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    fHitsCollection = new LCHitsCollection(fSDName, collectionName[0]);
    HCE->AddHitsCollection(fCollectionID, fHitsCollection);

    //Reset the hit map
    hitMap->clear();
}

//Pass the hit collection to the other G4 hits collection
//These collections will be transformed into the LCIO Collection you find in the output file
void LCSensitiveDetector::EndOfEvent(G4HCofThisEvent *HCE){
    LCHitMapIterator iter;

    for(iter=hitMap->begin(); iter!=hitMap->end(); ++iter) fHitsCollection->insert(iter->second);

    hitMap->clear(); 
    
    // // Add hit collection to HCE
    // HCE->AddHitsCollection(fCollectionID, fHitsCollection);
}



G4bool LCSensitiveDetector::FindHit(G4int cellID, G4double edep){
    /*Parameter explanations:
    * cellID = unique cell identifier
    * edep = amount of energy being added to the calorimeter cell
    * TID = track ID of the particle (from Step)
    * PDG = particle data group identifier code
    */

    // fg: use find method of map!
    // the find method of map returns:
    // successful - a pointer to the appropriate pair
    // unsuccessful - a pointer to the end marker(end())
    LCHitMapIterator iter = hitMap->find(cellID);

    // If you find the hit already in hitMap (as indexed by the cellID) - increment its energy!
    if(iter != hitMap->end()){
        (iter->second)->AddEdep(edep); 
        return true;
    }
    return false;
}
