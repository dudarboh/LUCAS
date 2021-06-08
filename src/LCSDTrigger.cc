#include "LCSDTrigger.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSDTrigger::LCSDTrigger(const G4String& name, const G4String& hitsCollectionName)
:G4VSensitiveDetector(name),
fHitsCollection(nullptr){
    collectionName.insert(hitsCollectionName);
}

LCSDTrigger::~LCSDTrigger(){}

void LCSDTrigger::Initialize(G4HCofThisEvent *hce){
    // std::cout<<"Start of LCSDTrigger::Initialize"<<std::endl;
    fHitsCollection = new LCTriggerHC(SensitiveDetectorName, collectionName[0]);
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(hcID, fHitsCollection);
    // Add only 1 trigger hit
    fHitsCollection->insert(new LCHitTrigger());
    // std::cout<<"End of LCSDTrigger::Initialize"<<std::endl;
}

G4bool LCSDTrigger::ProcessHits(G4Step *step, G4TouchableHistory*){
    // std::cout<<"Start of LCSDTrigger::ProcessHits"<<std::endl;
    G4double e_dep = step->GetTotalEnergyDeposit();
    if (e_dep <= 0.) return true;
    LCHitTrigger *hit = (*fHitsCollection)[0];
    auto name = step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName();
    if (name == "Sc1") hit->trigger1 = 1;
    if (name == "Sc2") hit->trigger2 = 1;
    if (name == "Sc3") hit->trigger3 = 1;
    hit->n_triggers = hit->trigger1 + hit->trigger2 + hit->trigger3;
    // std::cout<<"End of LCSDTrigger::ProcessHits"<<std::endl;
    return true;
}
