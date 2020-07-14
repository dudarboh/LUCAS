#ifndef LCDetectorConstruction_h
#define LCDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MagneticField.hh"
#include "G4FieldManager.hh"

class G4VPhysicalVolume;
class G4Material;
class G4VSensitiveDetector;
class G4VisAttributes;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction();
        virtual ~LCDetectorConstruction();

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();
    private:

        G4LogicalVolume* fLogicCalPad;
        G4LogicalVolume* fLogicTr1Pad;
        G4LogicalVolume* fLogicTr2Pad;
        G4LogicalVolume* fLogicSc1;
        G4LogicalVolume* fLogicSc2;
        G4LogicalVolume* fLogicSc3;

        #ifdef RUN_PH
            static G4ThreadLocal G4UniformMagField* fMagField;
            static G4ThreadLocal G4FieldManager* fFieldMgr;
            G4LogicalVolume* fLogicMagnet;
        #endif
};


#endif
