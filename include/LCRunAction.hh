#ifndef LCRunAction_h
#define LCRunAction_h 1

#include "G4UserRunAction.hh"
#include "LCPrimaryGeneratorAction.hh"
#include "LCHit.hh"

#include <chrono>
#include <vector>

class G4Run;

extern std::chrono::time_point<std::chrono::system_clock> launchTime;

class LCRunAction : public G4UserRunAction{
    public:
        LCRunAction();
        virtual ~LCRunAction();

        virtual void BeginOfRunAction(const G4Run*);
        virtual void EndOfRunAction (const G4Run*);

        void FillEventData(const G4Event* event, LCHitsCollection *HitsCollection);
        void ClearVectors();
        void SimulateNoise();

        std::vector<G4int> hit_sector;
        std::vector<G4int> hit_pad;
        std::vector<G4int> hit_layer;
        std::vector<G4double> hit_energy;
        std::vector<G4int> n_bs_particles;
        std::vector<G4int> n_dir_particles;

        G4double fApvNoise[4][64][8] = {};
};

#endif
