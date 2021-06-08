#ifndef LCDetectorConstruction_h
#define LCDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction(){};
        virtual ~LCDetectorConstruction(){};

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();

    private:
    G4LogicalVolume* fLogicCalPad;
    G4LogicalVolume* fLogicTr1Pad;
    G4LogicalVolume* fLogicTr2Pad;
    G4LogicalVolume* fLogicSc1;
    G4LogicalVolume* fLogicSc2;
    G4LogicalVolume* fLogicSc3;
    G4LogicalVolume* fLogicMagnet;

};


#endif
