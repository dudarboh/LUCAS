#ifndef LCEVENTDATA_HH_
#define LCEVENTDATA_HH_ 1

#include <vector>
#include "G4Event.hh"
#include "LCHit.hh"

class LCEventData{
    public:
        LCEventData();
        ~LCEventData();

        void FillEventData(const G4Event* event, LCHitsCollection *HitsCollection);
        void Clear();

        std::vector<G4int> hit_sector;
        std::vector<G4int> hit_pad;
        std::vector<G4int> hit_layer;
        std::vector<G4double> hit_energy;
        std::vector<G4int> hit_bs;
};

#endif
