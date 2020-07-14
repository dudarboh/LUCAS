#ifndef LCHitTrigger_h
#define LCHitTrigger_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Types.hh"

class LCHitTrigger:public G4VHit{
    public:
        LCHitTrigger();

        virtual ~LCHitTrigger();

        inline void* operator new(size_t);
        inline void  operator delete(void*);

        G4int n_triggers, trigger1, trigger2, trigger3;
};

// You just need this part. Dont change it
using LCTriggerHC = G4THitsCollection<LCHitTrigger>;

extern G4ThreadLocal G4Allocator<LCHitTrigger>* LCHitTriggerAllocator;

inline void* LCHitTrigger::operator new(size_t){
    if(!LCHitTriggerAllocator) LCHitTriggerAllocator = new G4Allocator<LCHitTrigger>;
    return (void*)LCHitTriggerAllocator->MallocSingle();
}

inline void LCHitTrigger::operator delete(void *hit){
    LCHitTriggerAllocator->FreeSingle((LCHitTrigger*)hit);
}

#endif
