#include "LCHitTrigger.hh"

G4ThreadLocal G4Allocator<LCHitTrigger>* LCHitTriggerAllocator = 0;

LCHitTrigger::LCHitTrigger()
:G4VHit(),
n_triggers(0),
trigger1(0),
trigger2(0),
trigger3(0)
{}

LCHitTrigger::~LCHitTrigger(){}
