#ifndef LCField_H
#define LCField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

class LCField : public G4MagneticField{
    public:
        LCField();
        ~LCField();

        void GetFieldValue(const double Point[3], double *Bfield) const;

    private:
        G4double Bz;
        G4double rmax_sq;
        G4double zmax;
};

#endif
