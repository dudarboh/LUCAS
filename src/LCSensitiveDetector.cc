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

    //Calculate hit's characteristics
    G4ThreeVector GlobalHitPos = ((step->GetPreStepPoint()->GetPosition()) + (step->GetPostStepPoint()->GetPosition())) / 2.;
    G4double x = GlobalHitPos.x();
    G4double y = GlobalHitPos.y();
    G4double z = GlobalHitPos.z();

    G4TouchableHandle touchable = step->GetPreStepPoint()->GetTouchableHandle();

    // Layer
    G4int layer = touchable->GetCopyNumber(1);
    // Pad
    G4ThreeVector LocalHitPos = touchable->GetHistory()->GetTopTransform().TransformPoint(GlobalHitPos);
    G4int pad = (G4int)floor((LocalHitPos.getRho() - fCalRhoMin) / fCellDimRho);
    // sector
    G4double phi = LocalHitPos.getPhi();
    G4int sector = (G4int)floor((phi - 5./12. * M_PI) / fCellDimPhi + 1);
 
    G4int pdg = step->GetTrack()->GetDefinition()->GetPDGEncoding();
    G4int direction;
    if(step->GetTrack()->GetMomentum().z() >= 0) direction = 1;
    else direction = -1;

    G4int bornInSi;
    if(step->GetPreStepPoint()->GetMaterial()->GetName() == "Si") bornInSi = 1;
    else bornInSi = 0;

    if(pad<0 || pad>fNumCellsRho){
        G4ExceptionDescription msg;
        msg<<"Hit pad is weird"<<pad; 
        G4cout<<"rho: "<<LocalHitPos.getRho()<<"  Min: "<<fCalRhoMin<< "  dimrho:  "<<fCellDimRho<<std::endl;
        G4Exception("B4cCalorimeterSD::ProcessHits()", "pad", FatalException, msg);
    }
    if(sector < 1 || sector > 4 || phi < 5./12. * M_PI || phi > 7./12. * M_PI){
        G4ExceptionDescription msg;
        msg<<"Hit sector is weird: "<<sector<<std::endl; 
        G4Exception("B4cCalorimeterSD::ProcessHits()", "sector", FatalException, msg);
    }

    fHitsCollection->insert(new LCHit(x, y, z, sector, pad, layer, edep, direction, bornInSi, pdg));
    return true;
}
