/*
 * LCDetectorConstruction.hh
 * LumiCal
 *
 *  Created on: Mar 2, 2009
 *      Author: aguilar
 *
 */


#include <cstdlib> 
#ifndef LCDETECTORCONSTRUCTION_HH_
#define LCDETECTORCONSTRUCTION_HH_

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4PVDivision.hh"
#include "G4VPVParameterisation.hh"
//#include "G4VNestedParameterisation.hh"
#include "G4NistManager.hh"

#include "LCSensitiveDetector.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Region;

class LCDetectorConstruction : public G4VUserDetectorConstruction
{
    // Constructor and destructors
public:
    LCDetectorConstruction();
    ~LCDetectorConstruction();


// ================ Volumes ================

public:

  G4VPhysicalVolume *Construct();
  void CreateTheWorld();
  void InitDetectorParameters();
  void BuildField();
  void BuildBeamPipe();
  void BuildLCal();
  void BuildLHcal();
  void BuildBCal();
  void BuildMask();
  void BuildTBeam();
  void BuildTBeamPT();
  void BuildTBeamPT16();
  void SetRegionCuts();
  void Print();

private:
    // -------------- WORLD VOLUME
    G4LogicalVolume     *logicWorld;     // world logical volume
    G4VPhysicalVolume   *physiWorld;     // world physical volume

    // -------------- Beam Pipe
    G4LogicalVolume *logicLCalInnerTube, *logEndVac;

 
    // -------------- LUMICAL SHELL
    G4LogicalVolume     *logicWholeLC;   // LumiCal logical volume

    // -------------- LAYER ( = SENSORS + TUNGSTEN ABSORBER + SUPPORT )
    G4LogicalVolume     *logicLayer;     // cell logical volume
    // this volume is designed to put connect the sensors and support layer
    // with the tungsten plate and then to be iterated inside the LumiCal
    // (WholeLC) volume

    // -------------- TUNGSTEN ABSORBER PLATE
    G4LogicalVolume     *logicAbsorber;  // plate logical volume

    // -------------- FANOUT LAYERS
    G4LogicalVolume     *logicFanoutFrnt; // front fanout log vol

    G4LogicalVolume     *logicFanoutBack;  // back fanout log vol

    // -------------- SENSOR
  G4LogicalVolume     *logicSensor, *logicSensorV, *logicSensorEnv;    // Sensor logical volume
    // -------------- Metallization mother volume
  G4LogicalVolume     *logicMetSec, *logicMetalV;   
    G4LogicalVolume     *MetSector1, *MetSector2, *MetSector4; 

    // -------------- SECTOR
  G4LogicalVolume     *logicFESector, *logicChip, *logicPCB;    
    G4LogicalVolume     *logicSector1, *logicSector2, *logicSector4; 
    // 48 sectors per sensor; contains cells

    // -------------- CELL
    G4LogicalVolume     *logicCell;         // cell logical volume
    G4LogicalVolume     *logicCellMet;      // pad metallization volume
    // 64 cells per sector
     // -------------- FE electronics
    G4LogicalVolume     *logicFEmother;      // FE chips mother volume 
  
 

// ================ Materials ================

private:

    G4Material  *Vacuum,                // World
                *Air,                   // Filler
                *PLASTIC_SC,
                *Silicon,               // Cell
                *Aluminium,             // Cell metallization
                *BeamPipeMat,           // central Beam pipe mat
                *Iron,                  // lateral Beam pipe mat
                *Tungsten,              // Absorber plate
                *Copper,                // Fanout component
                *Graphite,              // BCal front shield
                *Kapton,                // Fanout component
                *Epoxy,                 // Fanout component
                *FanoutMatF,            // Front Fanout material
                *FanoutMatB,            // Back Fanout material
                *LHcalMat,
                *BCalMat,
                *Mask_Mat,
                *WorldMat;
  // G4 regions created to set production cuts
  // 
  G4Region *regionLCal, *regionBCal, *regionLHcal, *regionMask;
  G4bool VirtualCell;
  // rotation angle
   G4double rotAng, rotAng1, rotAng2;
  // geometric parameters
   G4double Lcal_zbegin;
   G4double Lcal_zend;
   G4double innerRad;
   G4double outerRad;
   G4double deadPhi;
   G4int    nLayers;
   G4int    nTiles;
   G4int    nSectors;
   G4int    nCells;
   G4double startPhi;
   G4double endPhi;
   G4double phi_offset;
   G4double Zcave;
   G4double CaveDepth;
   G4double hLumiCalDZ;          // half length of LCAL
   G4double Cell0_Rad;
   G4double SensRadMin;        // silicon sensor r-min
   G4double SensRadMax;        // silicon sensor r-max
   G4double SensZ0;
   G4double SensDZ ;           // z distance between sensors
   G4double CellPitch;
   G4double hLayerDZ;         // half thickness of the layer
   G4double layer_gap;         // air gap between layers
   G4double hSiliconDZ;       // half thickness of the siliconZ
   G4double hMetalDZ;         // half thickness of pad metallization
   G4double hSensorDZ;
   G4double hFanoutFrontDZ;       // half thickness fanout front
   G4double hFanoutBackDZ;       // half thickness fanout back 
   G4double hTungstenDZ;      // half thickness absorber
   G4double hAbsorberPitchDZ;// pitch between absorber layer defulte 1 mm 
   G4double sectorPhi;
   G4double tilePhi;
   G4double FECave_hDZ;
   G4double FECave_rmin ;
   G4double FECave_rmax ;
   G4double FEChip_hDZ;
   G4double PCB_hDZ;
  G4double Lcal_extra_size;
  // Beam pipe
  G4double Lcal_inner_pipe_zend;
  G4double pipe_th;
  G4double LcalToBeamTol;
  G4double BCal_inner_outube_rmax;
  G4double BCal_inner_intube_rmax;
  // LHcal
  G4double LHcalToLCalDist;
  G4double LHcal_zbegin;
  G4double LHcal_zend;
  G4double LHcal_hDZ;
  G4double LHcal_rmin;
  G4double LHcal_rmax;
  // BCal
  G4double BCalToLHcalDist;
  G4double BCal_rmin;
  G4double BCal_rmax;
  G4double BCal_zbegin;
  G4double BCal_dPairMoni;   
  G4double BCal_dgraphite;   
  G4double BCal_zlength;
  G4double BCal_sphi;    
  G4double BCal_dphi;    
  // mask
  G4double Mask_zstart;
  G4double Mask_thickness;
  G4double Mask_hDX;
  G4double Mask_hDY;
  G4double Mask_rmin_at_zstart; 
  //

// ================ Sensitive Detector ================

private:

    // Implement a sensitive detector object
    LCSensitiveDetector *SensDet;
};


// ================ Cell Parameterization Class ================

class LCCellParam : public G4VPVParameterisation
{
public:

    LCCellParam(G4int    NoCells,
                G4double startR,
                G4double endR,
		G4double SensHalfZ,
                G4double SihalfZ,
                G4double AlhalfZ,
                G4double clipSize,
                G4double startPhi,
                G4double deltaPhi);
    virtual ~LCCellParam();

public:

virtual  void ComputeTransformation(const G4int repNo, G4VPhysicalVolume *physVol) const;
virtual  void ComputeDimensions(G4Tubs &Cell, const G4int copyNo, const G4VPhysicalVolume* physvol) const;

private: // Dummy declarations to get rid of warnings
    void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Box&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*) const {}

private:

    G4int    lNoCells;
    G4int    lNoLayers;
    G4int    lNoSectors;
    G4double lstartR;
    G4double lendR;
    G4double lSenshalfZ;
    G4double lSihalfZ;
    G4double lAlhalfZ;
    G4double lstartPhi;
    G4double ldeltaPhi;
    G4double lclipSize;
    G4double ldeltaR;
};


#endif /* LCDETECTORCONSTRUCTION_HH_ */
