// LCDetectorConstruction.hh
// LumiCal
//
//  Created on: Mar 2, 2009
//      Author: aguilar


#ifndef LCDETECTORCONSTRUCTION_HH_
#define LCDETECTORCONSTRUCTION_HH_ 1

#include "LCSensitiveDetector.hh"
#include <cstdlib> 
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4PVDivision.hh"
#include "G4NistManager.hh"


class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction();
        ~LCDetectorConstruction();

        G4NistManager *materials = G4NistManager::Instance();

    public:
        G4VPhysicalVolume *Construct();
        void Sensor();
        void Absorber();
        void Slot();
        void World();

    private:
        G4LogicalVolume *logicWorld;
        G4VPhysicalVolume *physicWorld;
 
        G4LogicalVolume *logicSlot;

        G4LogicalVolume *logicSensor;

        G4LogicalVolume *logicAbsorber;
        G4LogicalVolume *logicFanoutFront;
        G4LogicalVolume *logicFanoutBack;
        G4LogicalVolume *logicAl;
        G4LogicalVolume *logicSi;

    private:
        G4Material *Air;
        G4Material *W;
        G4Material *Ni;
        G4Material *Cu;
        G4Material *matAbsorber;
        G4Material *Si;
        G4Material *Al;
        G4Material *Carbon;
        G4Material *Kapton;
        G4Material *Epoxy;
        G4Material *matFiber;
        G4Material *matFanoutFront;
        G4Material *matFanoutBack;

        G4Element *H;
        G4Element *C;
        G4Element *O;

        G4Material *PLASTIC_SC;

    private:
        G4double xSensorSize;
        G4double ySensorSize;
        G4double zSensorSize;

        G4double rSensorMin;
        G4double rSensorMax;
        G4double rSensorGap;
        G4int nSensorPads;

        G4double xSlotSize;
        G4double ySlotSize;
        G4double zSlotSize;

        G4double xAbsorberSize;
        G4double yAbsorberSize;
        G4double zAbsorberSize;

    private:
        // Implement a sensitive detector object
        LCSensitiveDetector *SensDet;
};

#endif