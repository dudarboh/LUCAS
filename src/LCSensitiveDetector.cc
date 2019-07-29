#include "LCSensitiveDetector.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

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

    G4double edep = step->GetTotalEnergyDeposit();

    if(edep == 0.) return false;
    //Calculate hit's sector, pad, layer
    G4ThreeVector GlobalHitPos = ((step->GetPreStepPoint()->GetPosition()) + (step->GetPostStepPoint()->GetPosition())) / 2.;
    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();

    // Layer
    G4int layer = touchable->GetCopyNumber(2);

    // Cell
    G4ThreeVector LocalHitPos = touchable->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);
    G4int cell = (G4int)floor((LocalHitPos.getRho() - fCalRhoMin) / fCellDimRho);
    if(cell<0 || cell>fNumCellsRho){
        G4ExceptionDescription msg;
        msg<<"Hit cell is weird"<<cell; 
        G4cout<<"rho: "<<LocalHitPos.getRho()<<"  Min: "<<fCalRhoMin<< "  dimrho:  "<<fCellDimRho<<std::endl;
        G4Exception("B4cCalorimeterSD::ProcessHits()", "cell", FatalException, msg);
    }

    // sector
    G4double phi = LocalHitPos.getPhi();
    G4int sector = (G4int)floor((phi - 5./12. * M_PI) / fCellDimPhi + 1);
    if(sector < 1 || sector > 4 || phi < 5./12. * M_PI || phi > 7./12. * M_PI){
        G4ExceptionDescription msg;
        msg<<"Hit sector is weird: "<<sector<<std::endl; 
        G4Exception("B4cCalorimeterSD::ProcessHits()", "sector", FatalException, msg);
    }

    // Check if hit for this position already exists:
    LCHit *hit = new LCHit(sector, cell, layer, edep);

    G4bool hitExist = false;
    for(G4int i=0; i<fHitsCollection->entries(); i++){
        auto existed_hit = (*fHitsCollection)[i];
        if(existed_hit == hit){
            existed_hit->Add(edep);
            hitExist = true;
            break;
        }
    }
    if(!hitExist) fHitsCollection->insert(new LCHit(sector, cell, layer, edep));
    return true;
}

void LCSensitiveDetector::EndOfEvent(G4HCofThisEvent *){}
