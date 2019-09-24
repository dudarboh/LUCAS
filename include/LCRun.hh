#ifndef LCRun_h
#define LCRun_h 1

#include "LCHit.hh"

#include "G4Run.hh"
#include "g4root.hh"

#include <vector>

class LCRun:public G4Run{
    public:
        LCRun();
        virtual ~LCRun();
        virtual void RecordEvent(const G4Event*);
        void SimulateNoise();
    private:
        std::vector<G4int> hit_sector;
        std::vector<G4int> hit_pad;
        std::vector<G4int> hit_layer;
        std::vector<G4double> hit_energy;
        std::vector<G4int> hit_primary;

        G4double fApvNoise[4][64][8] = {};
};

#endif