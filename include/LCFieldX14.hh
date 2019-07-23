#ifndef LCFieldX14_H
#define LCFieldX14_H 1

#include <fstream>
#include "globals.hh"
#include "G4MagneticField.hh"

class LCFieldX14 : public G4MagneticField{
    public:
        LCFieldX14(G4String evfile);
        ~LCFieldX14();

        void GetFieldValue(const  double Point[3], double *Bfield) const;

    private:
        G4double rmax_sq;
        G4double zmax;
        G4String fileName;
        std::ifstream inputFile;
    protected:
        G4double zStep,xTan,zPos[1000],Bz[1000],Br[1000],Bx[1000];
};

#endif
