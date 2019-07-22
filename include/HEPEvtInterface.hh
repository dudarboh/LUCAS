#ifndef HEPEvtInterface_h
#define HEPEvtInterface_h 1

#include <fstream>
#include <vector>
#include "globals.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4HEPEvtParticle.hh"

class G4PrimaryVertex;
class G4Event;

// class description:
// This is a concrete class of G4VPrimaryGenerator.
// This class object reads an ASCII file which contains particles generated
// by a physics generator which supports /HEPEVT/ common block.
//
//  The format of ASCII file must be equivalent to the following sample fortran
// code.
//***********************************************************
//      SUBROUTINE HEP2G4
//*
//* Output /HEPEVT/ event structure to G4HEPEvtInterface
//*
//***********************************************************
//      PARAMETER (NMXHEP=2000)
//      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
//     >JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
//      DOUBLE PRECISION PHEP,VHEP
//*
//      WRITE(6,*) NHEP
//      DO IHEP=1,NHEP
//       WRITE(6,10) 
//     >  ISTHEP(IHEP),IDHEP(IHEP),JDAHEP(1,IHEP),JDAHEP(2,IHEP),
//     >  PHEP(1,IHEP),PHEP(2,IHEP),PHEP(3,IHEP),PHEP(5,IHEP)
//10    FORMAT(4I5,4(1X,D15.8))
//      ENDDO
//*
//      RETURN
//      END
//
//  The position and time of the primary interaction must be set by the corresponding
// set methods of G4VPrimaryGenerator base class, otherwise zero will be set.
//

class HEPEvtInterface:public G4VPrimaryGenerator{
    public: // with description
        HEPEvtInterface(char* evfile);
        HEPEvtInterface(G4String evfile);
        // Constructors, "evfile" is the file name (with directory path).
    public:
        ~HEPEvtInterface();
        void GeneratePrimaryVertex(G4Event* evt);

    private:
        G4String fileName;
        std::ifstream inputFile;
        std::vector<G4HEPEvtParticle*> HPlist;
};

#endif

