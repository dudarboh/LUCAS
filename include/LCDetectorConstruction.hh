#ifndef LCDetectorConstruction_h
#define LCDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

#include "G4LogicalVolume.hh"

class G4VPhysicalVolume;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction();
        virtual ~LCDetectorConstruction();

        virtual G4VPhysicalVolume *Construct();
        virtual void ConstructSDandField();

    private:
        void fancyVisualization();
        void buildEpoxy();
        void constructSensor();
        void constructTB16();
        void construct20Planes();

        // Build logical volumes
        G4LogicalVolume *buildWorld();
        G4LogicalVolume *buildSensor();
        G4LogicalVolume *buildAbsorberPL();
        G4LogicalVolume *buildAbsorberMSG();
        G4LogicalVolume *buildFanout(G4String logicName, G4double zEpoxy, G4double zKapton, G4double zCu);
        G4LogicalVolume *buildAl();
        G4LogicalVolume *buildSi();
        G4LogicalVolume *buildSc2();
        G4LogicalVolume *buildSc3();

        G4LogicalVolume *logicWorld;
        G4VPhysicalVolume *physicWorld;
        G4LogicalVolume *logicAbsorberPL;
        G4LogicalVolume *logicAbsorberMSG;
        G4LogicalVolume *logicSensor;
        G4LogicalVolume *logicFanoutFront;
        G4LogicalVolume *logicFanoutBack;
        G4LogicalVolume *logicSi;
        G4LogicalVolume *logicAl;
        G4LogicalVolume *logicSc2;
        G4LogicalVolume *logicSc3;

        G4double zSlot = 4.5*mm;
        G4double zSensor = 0.79*mm;
        G4double zAbsorberPL = 3.5*mm;
        G4double zAbsorberMSG = 3.57*mm;

        G4double zFanout = 0.15*mm;
        G4double zAl = 0.02*mm;
        G4double zSi = 0.32*mm;

        G4double rSensorMin = 80.*mm;
        G4double rSensorMax = 195.2*mm;
        G4double rSensorGap = 0.*mm; // default 1.2mm I dont know which to choose :c
};

#endif