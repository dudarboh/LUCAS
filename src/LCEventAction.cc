#include "LCEventAction.hh"
#include "LCHitCalorimeter.hh"
#include "LCHitTracker.hh"
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

namespace {
    // Utility function which finds a hit collection with the given Id
    // and print warnings if not found
    G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
        auto hce = event->GetHCofThisEvent();
        if(!hce){
            G4ExceptionDescription msg;
            msg << "No hits collection of this event found." << G4endl;
            G4Exception("LCEventAction::EndOfEventAction()", "LCCode001", JustWarning, msg);
            return nullptr;
        }
        auto hc = hce->GetHC(collId);
        if(!hc){
            G4ExceptionDescription msg;
            msg << "Hits collection " << collId << " of this event not found." << G4endl;
            G4Exception("LCEventAction::EndOfEventAction()", "LCCode001", JustWarning, msg);
        }
        return hc;
    }
}


LCEventAction::LCEventAction()
 : G4UserEventAction(),
   fCalHCID(-1),
   fTracker1HCID(-1),
   fTracker2HCID(-1),
   fTriggerHCID(-1){
       G4RunManager::GetRunManager()->SetPrintProgress(1);
}

LCEventAction::~LCEventAction(){}


void LCEventAction::BeginOfEventAction(const G4Event* /*event*/){
    // std::cout<<"Start of LCEventAction::BeginOfEventAction"<<std::endl;
    // Find hit collections by names (just once)
    // and save them in the data members of this class
    if( (fCalHCID == -1)
    || (fTracker1HCID == -1)
    || (fTracker2HCID == -1)
    || (fTriggerHCID == -1)){
        auto sdManager = G4SDManager::GetSDMpointer();

        // Get HCID with the following HC names
        fCalHCID = sdManager->GetCollectionID("Calorimeter/LCCalHC");
        fTracker1HCID = sdManager->GetCollectionID("Tracker1/LCTrackerHC");
        fTracker2HCID = sdManager->GetCollectionID("Tracker2/LCTrackerHC");
        fTriggerHCID = sdManager->GetCollectionID("Trigger/LCTriggerHC");
    }

    // std::cout<<"CalHCID: "<<fCalHCID<<std::endl;
    // std::cout<<"Tracker1HCID: "<<fTracker1HCID<<std::endl;
    // std::cout<<"Tracker2HCID: "<<fTracker2HCID<<std::endl;
    // std::cout<<"TriggerHCID: "<<fTriggerHCID<<std::endl;
    // std::cout<<"End of LCEventAction::BeginOfEventAction"<<std::endl;
}

void LCEventAction::EndOfEventAction(const G4Event* event){
    // std::cout<<"Start of LCEventAction::EndOfEventAction"<<std::endl;
    // Fill ntuple
    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    //Analyse Trigger.
    auto trigger_hc = GetHC(event, fTriggerHCID);
    auto trigger_hit = static_cast<LCHitTrigger*>(trigger_hc->GetHit(0));
    analysisManager->FillNtupleIColumn(0, trigger_hit->n_triggers);
    analysisManager->FillNtupleIColumn(1, trigger_hit->trigger1);
    analysisManager->FillNtupleIColumn(2, trigger_hit->trigger2);
    analysisManager->FillNtupleIColumn(3, trigger_hit->trigger3);
    // std::cout<<"LCEventAction::EndOfEventAction Trigger is filled"<<std::endl;

    ////////////////////////////////////////////////////////
    //Analyse Calorimeter
    auto cal_hc = GetHC(event, fCalHCID);

    auto CalNHits = 0;
    for (unsigned long i = 0; i < cal_hc->GetSize(); ++i){
        auto cal_hit = static_cast<LCHitCalorimeter*>(cal_hc->GetHit(i));
        if(cal_hit->energy <= 0.) continue;
        CalNHits++;
        fCalSector.push_back(cal_hit->sector);
        fCalPad.push_back(cal_hit->pad);
        fCalLayer.push_back(cal_hit->layer);
        fCalEnergy.push_back(cal_hit->energy);
    }
    analysisManager->FillNtupleIColumn(4, CalNHits);
    // std::cout<<"LCEventAction::EndOfEventAction Calorimeter is filled"<<std::endl;

    ////////////////////////////////////////////////////////
    //Analyse Tracker1
    auto tr1_hc = GetHC(event, fTracker1HCID);

    auto Tracker1Nhits = tr1_hc->GetSize();
    analysisManager->FillNtupleIColumn(9, Tracker1Nhits);

    for (unsigned long i = 0; i < Tracker1Nhits; ++i){
        auto tr1_hit = static_cast<LCHitTracker*>(tr1_hc->GetHit(i));
        fTr1Sector.push_back(tr1_hit->sector);
        fTr1Pad.push_back(tr1_hit->pad);
        fTr1Layer.push_back(tr1_hit->layer);
        fTr1Energy.push_back(tr1_hit->energy);
        fTr1Type.push_back(tr1_hit->type);
        fTr1TrackLen.push_back(tr1_hit->track_len);
        fTr1X.push_back(tr1_hit->x);
        fTr1Y.push_back(tr1_hit->y);
        fTr1Z.push_back(tr1_hit->z);
        fTr1Px.push_back(tr1_hit->px);
        fTr1Py.push_back(tr1_hit->py);
        fTr1Pz.push_back(tr1_hit->pz);
        fTr1PEnergy.push_back(tr1_hit->p_energy);
    }
    // std::cout<<"LCEventAction::EndOfEventAction Tracker1 is filled"<<std::endl;

    ////////////////////////////////////////////////////////
    //Analyse Tracker2
    auto tr2_hc = GetHC(event, fTracker2HCID);

    auto Tracker2Nhits = tr2_hc->GetSize();
    analysisManager->FillNtupleIColumn(23, Tracker2Nhits);

    for (unsigned long i = 0; i < Tracker2Nhits; ++i){
        auto tr2_hit = static_cast<LCHitTracker*>(tr2_hc->GetHit(i));
        fTr2Sector.push_back(tr2_hit->sector);
        fTr2Pad.push_back(tr2_hit->pad);
        fTr2Layer.push_back(tr2_hit->layer);
        fTr2Energy.push_back(tr2_hit->energy);
        fTr2Type.push_back(tr2_hit->type);
        fTr2TrackLen.push_back(tr2_hit->track_len);
        fTr2X.push_back(tr2_hit->x);
        fTr2Y.push_back(tr2_hit->y);
        fTr2Z.push_back(tr2_hit->z);
        fTr2Px.push_back(tr2_hit->px);
        fTr2Py.push_back(tr2_hit->py);
        fTr2Pz.push_back(tr2_hit->pz);
        fTr2PEnergy.push_back(tr2_hit->p_energy);
    }
    // std::cout<<"LCEventAction::EndOfEventAction Tracker2 is filled"<<std::endl;

    analysisManager->AddNtupleRow();
    ClearVectors();
    // std::cout<<"End of LCEventAction::EndOfEventAction"<<std::endl;
}

void LCEventAction::ClearVectors(){
    fCalSector.clear();
    fCalPad.clear();
    fCalLayer.clear();
    fCalEnergy.clear();

    fTr1Sector.clear();
    fTr1Pad.clear();
    fTr1Layer.clear();
    fTr1Energy.clear();
    fTr1Type.clear();
    fTr1TrackLen.clear();
    fTr1X.clear();
    fTr1Y.clear();
    fTr1Z.clear();
    fTr1Px.clear();
    fTr1Py.clear();
    fTr1Pz.clear();
    fTr1PEnergy.clear();

    fTr2Sector.clear();
    fTr2Pad.clear();
    fTr2Layer.clear();
    fTr2Energy.clear();
    fTr2Type.clear();
    fTr2TrackLen.clear();
    fTr2X.clear();
    fTr2Y.clear();
    fTr2Z.clear();
    fTr2Px.clear();
    fTr2Py.clear();
    fTr2Pz.clear();
    fTr2PEnergy.clear();
}
