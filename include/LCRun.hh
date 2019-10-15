#ifndef LCRun_h
#define LCRun_h 1

#include "LCTrHit.hh"
#include "LCCalHit.hh"

#include "G4Run.hh"
#include "g4root.hh"

#include <vector>
#include <chrono>
#include <ctime>

class LCRun:public G4Run{
    public:
        LCRun();
        virtual ~LCRun();
        virtual void RecordEvent(const G4Event*);
        void SimulateNoise();
    private:
        //Vectors to be written as branches in the output root file
        std::vector<G4int> tr1_hit_sector;
        std::vector<G4int> tr1_hit_pad;
        std::vector<G4int> tr1_hit_layer;
        std::vector<G4double> tr1_hit_energy;
        std::vector<G4int> tr1_hit_type;

        std::vector<G4double> tr1_particle_x;
        std::vector<G4double> tr1_particle_y;
        std::vector<G4double> tr1_particle_z;
        std::vector<G4double> tr1_particle_px;
        std::vector<G4double> tr1_particle_py;
        std::vector<G4double> tr1_particle_pz;
        std::vector<G4double> tr1_particle_energy;

        std::vector<G4int> tr2_hit_sector;
        std::vector<G4int> tr2_hit_pad;
        std::vector<G4int> tr2_hit_layer;
        std::vector<G4double> tr2_hit_energy;
        std::vector<G4int> tr2_hit_type;

        std::vector<G4double> tr2_particle_x;
        std::vector<G4double> tr2_particle_y;
        std::vector<G4double> tr2_particle_z;
        std::vector<G4double> tr2_particle_px;
        std::vector<G4double> tr2_particle_py;
        std::vector<G4double> tr2_particle_pz;
        std::vector<G4double> tr2_particle_energy;

        std::vector<G4int> cal_hit_sector;
        std::vector<G4int> cal_hit_pad;
        std::vector<G4int> cal_hit_layer;
        std::vector<G4double> cal_hit_energy;

        //This array stores APV channels noise sigmas
        // 4 sectors, 64 pads, 8 layers
        G4double fApvNoise[4][64][8] = {};

        // Launch time
        std::chrono::time_point<std::chrono::system_clock> fLaunchTime;
};

#endif