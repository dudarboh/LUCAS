// LCDetectorConstruction.hh
// LumiCal
//
//  Created on: Mar 2, 2009
//      Author: aguilar

#include <cstdlib> 
#ifndef LCDETECTORCONSTRUCTION_HH_
#define LCDETECTORCONSTRUCTION_HH_

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4PVDivision.hh"
#include "G4VPVParameterisation.hh"
#include "G4NistManager.hh"

#include "LCSensitiveDetector.hh"

class G4Box;
class G4Tubs;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4Region;

class LCDetectorConstruction : public G4VUserDetectorConstruction{
    public:
        LCDetectorConstruction();
        ~LCDetectorConstruction();

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
        void BuildTBeamPT16();
        void SetRegionCuts();
        void Print();

    // ================ Volumes ================
    private:
        // world
        G4LogicalVolume *logicWorld;
        G4VPhysicalVolume *physiWorld;

        // Beam Pipe
        G4LogicalVolume *logicLCalInnerTube;
        G4LogicalVolume *logEndVac;
 
        // LumiCal Shell
        G4LogicalVolume *logicWholeLC;

        // layer = SENSORS + TUNGSTEN ABSORBER + SUPPORT
        // This volume is sensor, tungsten plate and connection between them.
        // It is itterated inside LumiCal volume (logicWholeLC)
        G4LogicalVolume *logicLayer;

        // TUNGSTEN ABSORBER PLATE
        G4LogicalVolume *logicAbsorber;

        // Front and back FANOUT LAYERS
        G4LogicalVolume *logicFanoutFrnt;
        G4LogicalVolume *logicFanoutBack;

        // SENSOR
        G4LogicalVolume *logicSensor;
        G4LogicalVolume *logicSensorV;
        G4LogicalVolume *logicSensorEnv;

        // Metallization mother volume
        G4LogicalVolume *logicMetSec;
        G4LogicalVolume *logicMetalV;   
        G4LogicalVolume *MetSector1;
        G4LogicalVolume *MetSector2;
        G4LogicalVolume *MetSector4; 

        // SECTOR
        // 48 sectors per sensor; contains cells
        G4LogicalVolume *logicFESector;
        G4LogicalVolume *logicChip;
        G4LogicalVolume *logicPCB;
        G4LogicalVolume *logicSector1;
        G4LogicalVolume *logicSector2;
        G4LogicalVolume *logicSector4; 

        // Cell and pad metalization volumes
        // 64 cells per sector
        G4LogicalVolume *logicCell;
        G4LogicalVolume *logicCellMet;

        // FE chips electronics mother volume
        G4LogicalVolume *logicFEmother;

    // ================ Materials ==============
    private:
        G4Material  *Vacuum; // World
        G4Material  *Air; // Filler
        G4Material  *PLASTIC_SC;
        G4Material  *Silicon; // Cell
        G4Material  *Aluminium; // Cell metallization
        G4Material  *BeamPipeMat; // central Beam pipe mat
        G4Material  *Iron; // lateral Beam pipe mat
        G4Material  *Tungsten; // Absorber plate
        G4Material  *Copper; // Fanout component
        G4Material  *Graphite; // BCal front shield
        G4Material  *Kapton; // Fanout component
        G4Material  *Epoxy; // Fanout component
        G4Material  *FanoutMatF; // Front Fanout material
        G4Material  *FanoutMatB; // Back Fanout material
        G4Material  *LHcalMat;
        G4Material  *BCalMat;
        G4Material  *Mask_Mat;
        G4Material  *WorldMat;
        
        // G4 regions created to set production cuts
        G4Region *regionLCal;
        G4Region *regionBCal;
        G4Region *regionLHcal;
        G4Region *regionMask;

        G4bool VirtualCell;

        // rotation angle
        G4double rotAng, rotAng1, rotAng2;
        // geometric parameters
        G4double Lcal_zbegin;
        G4double Lcal_zend;
        G4double innerRad;
        G4double outerRad;
        G4double deadPhi;
        G4int nLayers;
        G4int nTiles;
        G4int nSectors;
        G4int nCells;
        G4double startPhi;
        G4double endPhi;
        G4double phi_offset;
        G4double Zcave;
        G4double CaveDepth;
        G4double hLumiCalDZ; // half length of LCAL
        G4double Cell0_Rad;
        G4double SensRadMin; // silicon sensor r-min
        G4double SensRadMax; // silicon sensor r-max
        G4double SensZ0;
        G4double SensDZ ; // z distance between sensors
        G4double CellPitch;
        G4double hLayerDZ; // half thickness of the layer
        G4double layer_gap; // air gap between layers
        G4double hSiliconDZ; // half thickness of the siliconZ
        G4double hMetalDZ; // half thickness of pad metallization
        G4double hSensorDZ;
        G4double hFanoutFrontDZ; // half thickness fanout front
        G4double hFanoutBackDZ; // half thickness fanout back 
        G4double hTungstenDZ; // half thickness absorber
        G4double hAbsorberPitchDZ; // pitch between absorber layer defulte 1 mm 
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

    // ================ Sensitive Detector =====
    private:
    // Implement a sensitive detector object
        LCSensitiveDetector *SensDet;
};


// ================ Cell Parameterization Class ================
class LCCellParam : public G4VPVParameterisation{
    public:
        LCCellParam(G4int NoCells,
                    G4double startR,
                    G4double endR,
                    G4double SensHalfZ,
                    G4double SihalfZ,
                    G4double AlhalfZ,
                    G4double clipSize,
                    G4double startPhi,
                    G4double deltaPhi);
        virtual ~LCCellParam();
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

        G4int lNoCells;
        G4int lNoLayers;
        G4int lNoSectors;
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

#endif
