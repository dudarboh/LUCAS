#include "LCSensitiveDetector.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"

LCSensitiveDetector::LCSensitiveDetector(const G4String& name,
                                        const G4String& hitsCollectionName,
                                        G4double rmin,
                                        G4double cRho,
                                        G4double cPhi,
                                        G4int nCellRho)
        :G4VSensitiveDetector(name),
        fHitsCollection(nullptr),
        fCalRhoMin(rmin),
        fCellDimRho(cRho),
        fCellDimPhi(cPhi),
        fNumCellsRho(nCellRho){

    collectionName.insert(hitsCollectionName);
}

LCSensitiveDetector::~LCSensitiveDetector(){}

void LCSensitiveDetector::Initialize(G4HCofThisEvent *hce){
    fHitsCollection = new LCHitsCollection(SensitiveDetectorName, collectionName[0]);
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}


G4bool LCSensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory*){

    G4double energy = step->GetTotalEnergyDeposit();
    if(energy == 0.) return false;

    //Calculate hit's characteristics
    G4ThreeVector GlobalHitPos = step->GetPreStepPoint()->GetPosition();
    G4ThreeVector LocalHitPos = step->GetPreStepPoint()->GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);

    G4int pad = (G4int)floor((LocalHitPos.getRho() - fCalRhoMin) / fCellDimRho);
    G4int sector = (G4int)floor((LocalHitPos.getPhi() - 5./12. * M_PI) / fCellDimPhi + 1);
    G4int layer = step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(1);
 
    if(pad<0 || pad>fNumCellsRho){
        G4ExceptionDescription msg;
        msg<<"Hit pad is weird"<<pad; 
        G4cout<<"rho: "<<LocalHitPos.getRho()<<"  Min: "<<fCalRhoMin<< "  dimrho:  "<<fCellDimRho<<std::endl;
        G4Exception("B4cCalorimeterSD::ProcessHits()", "pad", FatalException, msg);
    }
    if(sector < 1 || sector > 4){
        G4ExceptionDescription msg;
        msg<<"Hit sector is weird: "<<sector<<std::endl; 
        G4Exception("B4cCalorimeterSD::ProcessHits()", "sector", FatalException, msg);
    }

    LCHit *hit = new LCHit(sector, pad, layer, energy);
    LCHit *existing_hit;

    G4bool isExist = false;
    for(G4int i=0; i<fHitsCollection->entries(); i++){
        existing_hit = (*fHitsCollection)[i];
        if(hit->fSector == existing_hit->fSector
            && hit->fPad == existing_hit->fPad
            && hit->fLayer == existing_hit->fLayer){
            existing_hit->AddEnergy(energy);
            isExist = true;
            break;
        }
    }
    if(!isExist) fHitsCollection->insert(hit);
    return true;
}
