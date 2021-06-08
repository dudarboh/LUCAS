#include "LCEventAction.hh"
#include "LCHit.hh"
#include "LCHitTrigger.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "g4root.hh"
#include "Randomize.hh"


LCEventAction::LCEventAction() : G4UserEventAction(), fHCID(-1), fTriggerHCID(-1){}


void LCEventAction::EndOfEventAction(const G4Event* event){
    // std::cout<<"Start of LCEventAction::EndOfEventAction"<<std::endl;

    if( fHCID == -1){
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID("LumicalHCName");
        fTriggerHCID = G4SDManager::GetSDMpointer()->GetCollectionID("TriggerHCName");
    }

    //Get hit collections
    auto trigger_hc = static_cast<LCTriggerHC*>(event->GetHCofThisEvent()->GetHC(fTriggerHCID));
    if ( ! trigger_hc ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << fTriggerHCID;
        G4Exception("LCEventAction::GetHitsCollection()", "RIP TriggerHC", FatalException, msg);
    }
    auto lc_hc = static_cast<LumicalHC*>(event->GetHCofThisEvent()->GetHC(fHCID));
    if ( ! lc_hc ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << fHCID;
        G4Exception("LCEventAction::GetHitsCollection()", "RIP LumicalHC", FatalException, msg);
    }

    // Fill ntuple
    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    //Analyse Trigger.
    auto trigger_hit = static_cast<LCHitTrigger*>(trigger_hc->GetHit(0));
    analysisManager->FillNtupleIColumn(0, trigger_hit->n_triggers);
    analysisManager->FillNtupleIColumn(1, trigger_hit->trigger1);
    analysisManager->FillNtupleIColumn(2, trigger_hit->trigger2);
    analysisManager->FillNtupleIColumn(3, trigger_hit->trigger3);
    // std::cout<<"LCEventAction::EndOfEventAction Trigger is filled"<<std::endl;

    ////////////////////////////////////////////////////////
    //Analyse LumiCal

    auto nHits = 0;
    for (unsigned long i = 0; i < lc_hc->GetSize(); ++i){

        auto hit = static_cast<LCHit*>(lc_hc->GetHit(i));
        if(hit->fTotalEdep <= 0.) continue;
        nHits++;
        fSector.push_back(hit->fSector);
        fPad.push_back(hit->fPad);
        fLayer.push_back(hit->fLayer);
        fIsPrimary.push_back(hit->fIsPrimary);
        fTotalEdep.push_back(hit->fTotalEdep);
        fTotalTrackLen.push_back(hit->fTotalTrackLen);
        G4int maxEdepIdx = std::max_element(hit->fMcContEdep.begin(), hit->fMcContEdep.end()) - hit->fMcContEdep.begin();
        // std::cout<< "Max dep idx: "<< max_edep_idx<<std::endl;
        // std::cout<< "Size: "<< hit->mc_pos.size()<<std::endl;
        fMcContId.push_back(hit->fMcContId[maxEdepIdx]);
        fMcContPosX.push_back(hit->fMcContPos[maxEdepIdx].x());
        fMcContPosY.push_back(hit->fMcContPos[maxEdepIdx].y());
        fMcContPosZ.push_back(hit->fMcContPos[maxEdepIdx].z());
        fMcContMomX.push_back(hit->fMcContMom[maxEdepIdx].x());
        fMcContMomY.push_back(hit->fMcContMom[maxEdepIdx].y());
        fMcContMomZ.push_back(hit->fMcContMom[maxEdepIdx].z());
        fMcContEnergy.push_back(hit->fMcContEnergy[maxEdepIdx]);
        fMcContEdep.push_back(hit->fMcContEdep[maxEdepIdx]);
        fMcContTrackLen.push_back(hit->fMcContTrackLen[maxEdepIdx]);
        fMcContPdg.push_back(hit->fMcContPdg[maxEdepIdx]);

    }
    analysisManager->FillNtupleIColumn(4, nHits);

    analysisManager->AddNtupleRow();
    ClearVectors();
    // std::cout<<"End of LCEventAction::EndOfEventAction"<<std::endl;
}

void LCEventAction::ClearVectors(){
    fSector.clear();
    fPad.clear();
    fLayer.clear();
    fIsPrimary.clear();
    fTotalEdep.clear();
    fTotalTrackLen.clear();
    fMcContId.clear();
    fMcContPosX.clear();
    fMcContPosY.clear();
    fMcContPosZ.clear();
    fMcContMomX.clear();
    fMcContMomY.clear();
    fMcContMomZ.clear();
    fMcContEnergy.clear();
    fMcContEdep.clear();
    fMcContTrackLen.clear();
    fMcContPdg.clear();
}
