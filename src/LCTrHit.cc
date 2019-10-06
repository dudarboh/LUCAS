#include "LCTrHit.hh"

G4ThreadLocal G4Allocator<LCTrHit>* LCTrHitAllocator = 0;


// Hit is the energy deposit in a pad with a certain sector, pad, layer and hit_energy.
LCTrHit::LCTrHit(G4int sector, G4int pad, G4int layer)
    :G4VHit(),
    hit_sector(sector),
    hit_pad(pad),
    hit_layer(layer),
    hit_n_particles(0),
    hit_energy(0.),
    hit_length(0.){}

LCTrHit::~LCTrHit(){
    particle_x.clear();
    particle_y.clear();
    particle_z.clear();
    particle_type.clear();
    particle_px.clear();
    particle_py.clear();
    particle_pz.clear();
    particle_energy.clear();
    particle_primary.clear();
}
