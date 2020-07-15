#include "LCActionInitialization.hh"
#include "LCPrimaryGeneratorAction.hh"
#include "LCRunAction.hh"
#include "LCEventAction.hh"

LCActionInitialization::LCActionInitialization(G4String OutputFileName)
:G4VUserActionInitialization(){
    fOutputFileName = OutputFileName;
}
LCActionInitialization::~LCActionInitialization(){}

void LCActionInitialization::BuildForMaster() const{
    LCEventAction* eventAction = new LCEventAction;
    SetUserAction(new LCRunAction(eventAction, fOutputFileName));
}

void LCActionInitialization::Build() const{
    // std::cout<<"Start of LCActionInitialization::Build()"<<std::endl;
    SetUserAction(new LCPrimaryGeneratorAction);
    LCEventAction* eventAction = new LCEventAction;
    SetUserAction(eventAction);
    SetUserAction(new LCRunAction(eventAction, fOutputFileName));
    // std::cout<<"End of LCActionInitialization::Build()"<<std::endl;
}
