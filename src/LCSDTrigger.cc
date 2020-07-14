#include "LCSDTrigger.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4String.hh"
#include "Randomize.hh"

LCSDTrigger::LCSDTrigger(G4String detector_name)
:G4VSensitiveDetector(detector_name),
fTriggerHCID(-1){
    collectionName.insert("LCTriggerHC");
}

LCSDTrigger::~LCSDTrigger(){}

void LCSDTrigger::Initialize(G4HCofThisEvent *HCE){
    // std::cout<<"Start of LCSDTrigger::Initialize"<<std::endl;
    fTriggerHC = new LCTriggerHC(SensitiveDetectorName, collectionName[0]);
    if (fTriggerHCID<0) fTriggerHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fTriggerHC);
    HCE->AddHitsCollection(fTriggerHCID, fTriggerHC);
    // Add only 1 trigger hit
    fTriggerHC->insert(new LCHitTrigger());
    // std::cout<<"End of LCSDTrigger::Initialize"<<std::endl;
}

G4bool LCSDTrigger::ProcessHits(G4Step *step, G4TouchableHistory*){
    // std::cout<<"Start of LCSDTrigger::ProcessHits"<<std::endl;
    G4double e_dep = step->GetTotalEnergyDeposit();
    if (e_dep <= 0.) return true;
    LCHitTrigger *hit = (*fTriggerHC)[0];
    auto name = step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName();
    if (name == "Sc1") hit->trigger1 = 1;
    if (name == "Sc2") hit->trigger2 = 1;
    if (name == "Sc3") hit->trigger3 = 1;
    hit->n_triggers = hit->trigger1 + hit->trigger2 + hit->trigger3;
    // std::cout<<"End of LCSDTrigger::ProcessHits"<<std::endl;
    return true;
}
