/*
 * LCSensitiveDetector.cc
 * 2ModLumiCal
 *
 *  Created on: Mar 26, 2009
 *      Author: aguilar
 */

// time optimization
#include <time.h>

#include "LCSensitiveDetector.hh"
#include "LCHit.hh"
#include "LCEventAction.hh"
#include "Setup.hh"

LCSensitiveDetector::LCSensitiveDetector(G4String sdname,
    G4double rmin,
    G4double phi0,
    G4double cRho,
    G4double cPhi,
    G4int nCellRho,
    G4int nCellPhi,
    G4bool cellvirtual)
    :G4VSensitiveDetector(sdname),
    HCID(-1),
    SDName(sdname), // this sets string SDName to sdname
    hitsColl(0),
    VirtualCell(cellvirtual){

    collName = SDName + "_HC"; // not dynamic - name becomes LumiCalSD_HC
    collectionName.insert(collName); // a G4VSensitiveDetector element
    origin = G4ThreeVector();
    hitMap = new LCHitMap;

    // set primary args - this is how you tell LCSensDet the geometric parameters of LumiCal
    CalRhoMin = rmin;
    
    CalPhiOffset = phi0;

    cellDimRho = cRho;
    assert(cellDimRho > 0);

    cellDimPhi = cPhi;
    assert(cellDimPhi > 0);

    SetNCellRho(nCellRho);
    SetNCellPhi(nCellPhi);
    G4cout<<"SD created <"<<SDName<<">"<<G4endl;
}

LCSensitiveDetector::~LCSensitiveDetector(){
    hitMap->clear();
    delete hitMap;
}


void LCSensitiveDetector::Initialize(G4HCofThisEvent *HCE){
    // Create a (G4) hit collection for this event
    hitsColl = new LCHitsCollection(SDName, collName);
    if(HCID < 0) HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collName);

    // Would be sufficient to add the collection at the end of the event, but doing it here suppresses a warning during compilation
    HCE->AddHitsCollection(HCID, hitsColl);
    primaryID = 0; 

    //Reset the hit map
    hitMap->clear();
}

//Pass the hit collection to the other G4 hits collection
//These collections will be transformed into the LCIO Collection you find in the output file
void LCSensitiveDetector::EndOfEvent(G4HCofThisEvent *HCE){
    LCHitMapIterator iter;

    // the hit map stores all the hits - they are inserted during ProcessHits()
    // This loop cycles through the hit map and adds them all to hitsColl
    for(iter=hitMap->begin(); iter!=hitMap->end(); ++iter) hitsColl->insert(iter->second);

    // hit map has already been copied to hitsColl but what about the cell ID? is that just lost?
    hitMap->clear(); 
    
    // Add hit collection to HCE
    HCE->AddHitsCollection(HCID, hitsColl);
}


G4bool LCSensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *){
    // LOCAL VARIABLES AND CHECKS

    // store energy deposition and momentum change from primary particles
    G4double edep = aStep->GetTotalEnergyDeposit();

    // make sure you aren't using geantinos
    if(edep <= 0 && aStep->GetTrack()->GetDefinition()->GetParticleType() != "geantino") return true;

    // PARTICLE INFORMATION
    G4int parentID = aStep->GetTrack()->GetParentID();
    G4double ToF = aStep->GetTrack()->GetGlobalTime();
    // Set primary particle track ID/PDG
    G4int PDG = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();

    if(parentID <= 0){
        G4int PID = aStep->GetTrack()->GetTrackID();
        if(PID != primaryID){
            primaryID = PID;
            primaryPDG= PDG;
        }
    }

    // Get location (volumes & coordinates) with PreStepPoint->TouchableHandle
    const G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    const G4StepPoint *postStepPoint = aStep->GetPostStepPoint();
    G4ThreeVector GlobalHitPos = ((preStepPoint->GetPosition()) + (postStepPoint->GetPosition())) / 2.;

    G4int LC_num = 0, layer_num = 0, sector_num = 0, cell_num = 0;
    // Use the touchable handle to get the volume hierarchy for the hit
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();

    // Find the unique volume path for a cell which has a hit
    // Which copy of LumiCal?
    LC_num = 1;
    layer_num = theTouchable->GetHistory()->GetVolume(1)->GetCopyNo();
        
    if(!VirtualCell){
        // Which sector inside the tile? (sectors are replicated *after* the cells)
        sector_num = theTouchable->GetReplicaNumber(1);
        // Which cell inside sector?
        cell_num = theTouchable->GetReplicaNumber(0);
    }
    else{
        G4ThreeVector LocalHitPos = theTouchable->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);
        G4double rho = LocalHitPos.getRho();
        G4double phi = LocalHitPos.getPhi();
        phi = (phi < 0.) ? phi + 2. * M_PI : phi; 
        cell_num = (G4int)floor(( rho - (CalRhoMin-cellDimRho/2.) ) / cellDimRho );
        cell_num = (cell_num < 0) ? 0 : cell_num; 
        cell_num = (cell_num >NumCellsRho) ? NumCellsRho : cell_num; 
        sector_num = (G4int)floor(phi / cellDimPhi) + 1;
    }
    // ENCODE THE HIT

    // Use bitwise operations to store the location of a cell in a single
    // 32-bit word
    // 4 bytes, 4 levels in the volume tree - 1 byte per volume
    // | LC number | Layer number | Sector number | Cell number |
    // note - this only works because no volume will have a number > 255 (2^8)
    // LumiCal only uses id0; id1 is a legacy from the Mokka version

    cell_code cellID;
    cellID.id0 = 0;
    cellID.id1 = 0; // 32 0's in a row per member

    cellID.id0 |= (cell_num << 0); // store the cell # in the lowest 8 digits
    cellID.id0 |= (sector_num << 8); // shift the sector # to the next byte up
    cellID.id0 |= (layer_num << 16); // shift the layer # to the next byte up
    cellID.id0 |= (LC_num << 24); // shift the LumiCal # to the highest byte

    // get local and global position using touchable
    // check if the cell has been hit before, and check the indices
    if(!FindHit(cellID.id0, edep, primaryID, PDG) && !((cell_num > NumCellsRho) || (sector_num > NumSectorsPhi))){
        // Global hit coordinates
        // FG: the local coordinate system of the cell replica is the same as for the disk
        // containing the the pad - rotated by phi of the phi replica, i.e.
        // the origin is at the center of the disk
        // hence the position of the cell center in its coordinate system is given by:
        G4ThreeVector localCellPos(CalRhoMin + ((G4double)cell_num) * cellDimRho, 0., 0.);

        if(VirtualCell) localCellPos.setPhi((0.5000 + ((G4double)sector_num - 1)) * cellDimPhi);
        else localCellPos.setPhi(0.5000 * cellDimPhi);

        // compute GlobalCellPos based on touchable with localCellPos
        G4ThreeVector GlobalCellPos = theTouchable->GetHistory()->GetTopTransform().Inverse().TransformPoint(localCellPos);

        // assert id w/in valid range using bitwise ops
        // (0xff = 255 in hex)
        assert((cellID.id0 & 0xff) <= 120);

        // Use all this information to create an LCHit instance
        LCHit *theHit = new LCHit(GlobalHitPos.x(),
            GlobalHitPos.y(),
            GlobalHitPos.z(),
            GlobalCellPos.x(), // GlobalCellPosRho
            GlobalCellPos.y(), // GlobalCellPosPhi
            GlobalCellPos.z(), // GlobalCellPosZ
            edep,
            primaryID, // track number
            primaryPDG,
            cellID,
            ToF); // time since event begin

        //Insert the hit and cellID into the hitmap
        hitMap->insert(LCHitMapPair(cellID.id0, theHit));
    }
    return true;
}

G4bool LCSensitiveDetector::FindHit(G4int cellID, G4double edep, G4int TID, G4int PDG){
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
        (iter->second)->AddEdep(TID, PDG, edep); 
        return true;
    }
    return false;
}

void LCSensitiveDetector::SetNCellRho(G4int nx){
    // set number of cellS in a sector
    NumCellsRho = nx;
    assert(nx > 0);
}

void LCSensitiveDetector::SetNCellPhi(G4int ny){
    // set number of sectors in a layer (= # cells in phi dir)
    NumSectorsPhi = ny;
    assert(ny > 0);
}

void LCSensitiveDetector::clear(){}
