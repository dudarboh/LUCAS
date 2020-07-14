#include "LCActionInitialization.hh"
#include "LCPrimaryGeneratorAction.hh"
#include "LCRunAction.hh"
#include "LCEventAction.hh"

LCActionInitialization::LCActionInitialization():G4VUserActionInitialization(){}
LCActionInitialization::~LCActionInitialization(){}

void LCActionInitialization::BuildForMaster() const{
    LCEventAction* eventAction = new LCEventAction;
    SetUserAction(new LCRunAction(eventAction));
}

void LCActionInitialization::Build() const{
    std::cout<<"Start of LCActionInitialization::Build()"<<std::endl;
    SetUserAction(new LCPrimaryGeneratorAction);
    LCEventAction* eventAction = new LCEventAction;
    SetUserAction(eventAction);
    SetUserAction(new LCRunAction(eventAction));
    std::cout<<"End of LCActionInitialization::Build()"<<std::endl;
}
