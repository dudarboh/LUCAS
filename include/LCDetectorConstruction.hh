#ifndef LCDetectorConstruction_h
#define LCDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4FieldManager.hh"
#include "G4UniformMagField.hh"

class G4VPhysicalVolume;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction(){};
        virtual ~LCDetectorConstruction(){};

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();

    private:

    static G4ThreadLocal G4UniformMagField* fMagneticField;
    static G4ThreadLocal G4FieldManager* fFieldMgr;

    G4LogicalVolume* fLogicCalPad;
    G4LogicalVolume* fLogicTr1Pad;
    G4LogicalVolume* fLogicTr2Pad;
    G4LogicalVolume* fLogicSc1;
    G4LogicalVolume* fLogicSc2;
    G4LogicalVolume* fLogicSc3;
    G4LogicalVolume* fLogicMagnet;

};


#endif
