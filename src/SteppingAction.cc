
#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "LCEventAction.hh"
#include "Setup.hh"

#include "G4SystemOfUnits.hh"


SteppingAction::SteppingAction( LCEventAction* evt ):eventaction( evt )
{
}

void SteppingAction::UserSteppingAction(const G4Step* )
{
  /*    
  G4String VolName = fpSteppingManager->GetfCurrentVolume()->GetName();
  if ( VolName == "Absorber" ) { 
    const G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
    G4int LC_num = theTouchable->GetHistory()->GetVolume(1)->GetCopyNo();     // LumiCal copy No
    G4int layer_num  = theTouchable->GetHistory()->GetVolume(2)->GetCopyNo(); // layer copy No
    if( layer_num == 30 && LC_num == 2 ) {
      //      cout << VolName << " " << LC_num << " " << layer_num << endl;
      G4ThreeVector track = aStep->GetTrack()->GetMomentum();
      fpSteppingManager->GetTrack()->SetTrackStatus( fStopAndKill );
      eventaction->AddLeakEnergy( track.mag() );
    }  

  }
  */
}
