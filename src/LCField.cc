#include "LCField.hh"
#include "Setup.hh"

LCField::LCField(){
    Bz = Setup::Nominal_Field_value;
    rmax_sq = sqr(Setup::world_hdy);
    zmax = Setup::world_hdz;
}

LCField::~LCField(){;}

void LCField::GetFieldValue(const double Point[3], double *Bfield) const{
    Bfield[0] = 0.;
    Bfield[1] = 0.;
    if(std::abs(Point[2]) < zmax && (sqr(Point[0])+sqr(Point[1])) < rmax_sq) Bfield[2] = Bz;
    else Bfield[2] = 0.;
}
