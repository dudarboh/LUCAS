#ifndef LCDetectorConstruction_h
#define LCDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Types.hh"
#include "G4LogicalVolume.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"


class G4VPhysicalVolume;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction();
        virtual ~LCDetectorConstruction();

        virtual G4VPhysicalVolume *Construct();
        virtual void ConstructSDandField();
    private:
        G4LogicalVolume *logicMagnet;
        static G4ThreadLocal G4UniformMagField* fMagField;
        static G4ThreadLocal G4FieldManager* fFieldMgr;
};


#endif