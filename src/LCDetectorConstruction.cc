//
// LCDetectorConstruction.cc
// LumiCal
//
//  Created on: Feb 27, 2009
//     Authors: Jonathan Aguilar
//              bogdan.pawlik@ifj.edu.pl
//---------------------------------------------
//      sensor stracture from front to back  :
//          1. 0.150 kapton front.
//          1.2. 0.010 epoxy glue
//          2.1. 0.020 AL on sensor
//          2.2. 0.320 Si sensor
//          2.3. 0.020 AL on sensor
//          3.1. 0.040 epoxy (condactive - not take into acount)
//          3.2. 0.025 cupper (on kapton)
//          3.3. 0.065 kapton back
//          3.4. 0.020 epoxy glue
//          total 0.670  
//---------------------------------------------

#include "LCDetectorConstruction.hh"
#include "Setup.hh"
#include "LCSensitiveDetector.hh"
#include "LCField.hh"
#include "LCFieldX14.hh"
#include <iostream>
#include <sstream>
#include <string>
#include "globals.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
#include "G4Polyhedra.hh"
#include "G4EllipticalTube.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"

#include "G4TransportationManager.hh"
#include "G4FieldManager.hh"
#include "G4SDManager.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4SystemOfUnits.hh"


LCDetectorConstruction::LCDetectorConstruction()
    :logicWorld(0),
    physiWorld(0),
    logicWholeLC(0),
    logicSensor(0),
    logicCell(0){}

LCDetectorConstruction::~LCDetectorConstruction(){}

G4VPhysicalVolume* LCDetectorConstruction::Construct(){
    G4cout<<"LCDetectorConstrucion::Construct(): Building Detector SubSets ...."<<G4endl;
    InitDetectorParameters();

    CreateTheWorld();
    G4cout<<" Setup::LcalTBeam "<<Setup::LcalTBeam<<G4endl;

    //if 2016 desy test beam. I removed 2014 TB option from the code. It's 2k19 already.
    if(Setup::LcalTBeam > 0) BuildTBeamPT16(); 
    else{
        if(Setup::Nominal_Field_value > 0.) BuildField();
        if(Setup::Build_Beampipe == "Yes") BuildBeamPipe();

        BuildLCal();

        if(Setup::Build_LHcal == "Yes") BuildLHcal();
        if(Setup::Build_BCal == "Yes") BuildBCal();
        if(Setup::Build_Mask == "Yes") BuildMask();

        SetRegionCuts();
        Print();
    }
    G4cout<<"LCDetectorConstruction::Construct completed."<<G4endl;

    return physiWorld;
}

void LCDetectorConstruction::SetRegionCuts(){
    // LCAL
    G4ProductionCuts *cutsLCal = new G4ProductionCuts();
    cutsLCal->SetProductionCut(Setup::LCal_Region_Cut*mm);
    regionLCal->SetProductionCuts(cutsLCal);
    // LHCAL
    if(Setup::Build_LHcal == "Yes"){
        G4ProductionCuts *cutsLHcal = new G4ProductionCuts();
        cutsLHcal->SetProductionCut(Setup::LHcal_Region_Cut*mm);
        regionLHcal->SetProductionCuts(cutsLHcal);
    }
    // BCAL
    if(Setup::Build_BCal == "Yes"){
        G4ProductionCuts *cutsBCal = new G4ProductionCuts();
        cutsBCal->SetProductionCut(Setup::BCal_Region_Cut*mm);
        regionBCal->SetProductionCuts(cutsBCal);
    }
    // MASK
    if(Setup::Build_Mask == "Yes"){
        G4ProductionCuts *cutsMask = new G4ProductionCuts();
        cutsMask->SetProductionCut(Setup::Mask_Region_Cut*mm);
        regionMask->SetProductionCuts(cutsMask);
    }
}

void LCDetectorConstruction::InitDetectorParameters(){
    // Initialize LCAL local parameters
    Vacuum = Setup::Vacuum; // beam pipe filler
    Air = Setup::Air; // world filler
    PLASTIC_SC = Setup::PLASTIC_SC; // world filler
    Silicon = Setup::Silicon; // Sensor Cell
    Iron = Setup::Iron; // material for lateral beam tubes
    Aluminium = Setup::Alu; // material for LCAL mechanical parts
    Tungsten = Setup::Wabsorber; // Absorber plate
    Graphite = Setup::Graphite;
    FanoutMatF = Setup::FanoutMatF; // Front Fanout material
    FanoutMatB = Setup::FanoutMatB; // Back Fanout material
    BeamPipeMat = Setup::Beryllium;
    LHcalMat = Setup::Tungsten;
    BCalMat = Setup::Tungsten;
    Mask_Mat = Setup::Tungsten;
    if(Setup::Build_Beampipe == "Yes") WorldMat = Air;
    else WorldMat = Vacuum;
    if(Setup::LcalTBeam >0) WorldMat = Air;
    rotAng = Setup::Beam_Crossing_Angle / 2.;
    rotAng1 = 180.*deg - rotAng;
    rotAng2 = rotAng;

    // geometric parameters
    // LCAL
    VirtualCell = Setup::Lcal_virtual_cells; // cell type 
    nLayers = Setup::Lcal_n_layers;
    nSectors = Setup::Lcal_n_sectors;
    nTiles = Setup::Lcal_n_tiles;
    nCells = Setup::Lcal_n_rings;

    Lcal_zend = Setup::Lcal_z_end;
    innerRad = Setup::Lcal_inner_radius;
    Cell0_Rad = Setup::Lcal_Cell0_radius;
    SensRadMin = Setup::Lcal_SensRadMin; // silicon sensor r-min including dead edges
    SensRadMax = Setup::Lcal_SensRadMax; // silicon sensor r-max

    deadPhi = Setup::Lcal_sector_dead_gap;
    phi_offset = Setup::Lcal_layers_phi_offset;
    
    // derived
    hLumiCalDZ = Setup::Lcal_hdz; // half length of LCAL
    Lcal_zbegin = Lcal_zend - 2.*hLumiCalDZ ;
    SensDZ = Setup::Lcal_sensor_dz;
    CellPitch = Setup::Lcal_CellPitch;
    layer_gap = Setup::Lcal_layer_gap; // air gap between layers
    hSiliconDZ = Setup::Lcal_silicon_hdz; // half thickness of the silicon
    hMetalDZ = Setup::Lcal_pad_metal_thickness/2.; // half thickness of the pad metallization
    hFanoutFrontDZ = Setup::Lcal_fanoutF_hdz; // half thickness fanout front
    hFanoutBackDZ = Setup::Lcal_fanoutB_hdz; // half thickness fanout back 
    hTungstenDZ = Setup::Lcal_tungsten_hdz; // half thickness absorber
    hLayerDZ = Setup::Lcal_layer_hdz; // half thickness of the layer
    hSensorDZ = hSiliconDZ + hMetalDZ; // sensor half thickness including metallization
    hAbsorberPitchDZ = Setup::Lcal_absorber_pitch / 2.; // half pitch between absorber layer (defulte 1 mm) 
    startPhi = Setup::Lcal_start_phi;
    endPhi = Setup::Lcal_end_phi;
    sectorPhi = Setup::Lcal_sector_dphi;
    tilePhi = endPhi / G4double( nTiles );
    assert(SensRadMin >= (innerRad / cos(tilePhi /2.)));
    // FE - space
    FECave_hDZ = Setup::Lcal_ChipCaveDepth/2.;
    FECave_rmin = Setup::Lcal_SensRadMax;
    FECave_rmax = Setup::Lcal_FEChip_rmax;
    PCB_hDZ = Setup::Lcal_PCB_thickness / 2.;
    FEChip_hDZ = Setup::Lcal_FEChip_space / 2.; 
    G4double FE_size = FECave_rmax - FECave_rmin;
    Lcal_extra_size = (FE_size > Setup::Lcal_space_for_ears) ? FE_size : Setup::Lcal_space_for_ears;
    outerRad = SensRadMax + Lcal_extra_size;
    assert(FECave_hDZ >= (PCB_hDZ + FEChip_hDZ)); 
    // LHcal
    LHcalToLCalDist = 45.*mm;
    LHcal_zbegin = Lcal_zend + LHcalToLCalDist;
    LHcal_rmin = 93.0*mm;
    LHcal_rmax = 330.0*mm;
    LHcal_hDZ = (525.0 /2.)*mm;
    LHcal_zend = LHcal_zbegin + 2.*LHcal_hDZ;
    // BCal
    BCalToLHcalDist = 390.*mm;
    BCal_rmin = 25.*mm;
    BCal_rmax = 150.*mm;
    BCal_zbegin = LHcal_zend + BCalToLHcalDist;
    BCal_dPairMoni = 1.*mm;
    BCal_dgraphite = 100.*mm;
    BCal_zlength = 117.*mm;
    BCal_sphi = 200.*deg;
    BCal_dphi = 320.*deg;  
    // beam
    pipe_th = Setup::Beam_pipe_thickness;
    Lcal_inner_pipe_zend = BCal_zbegin - 5.0*mm; 
    LcalToBeamTol = Setup::Lcal_to_BeamPipe_clearance; 
    BCal_inner_outube_rmax = BCal_rmin - 2.*mm;
    BCal_inner_intube_rmax = 15. *mm;
    // mask
    Mask_zstart = LHcal_zend + 5.*mm;
    Mask_thickness = 50.*mm;
    Mask_hDX = 290.*mm;
    Mask_hDY = 290.*mm;
    Mask_rmin_at_zstart = Setup::Lcal_inner_radius + Setup::Lcal_to_BeamPipe_clearance;
}

void LCDetectorConstruction::CreateTheWorld(){
    // WORLD
    // Top level
    G4cout<<"LCDetectorConstrucion::Construct(): creating World ....";
    G4cout<<"The paramters : dx dy dz WorldMat = "<<Setup::world_hdx<<"- "<<Setup::world_hdy<<" -  "<<Setup::world_hdz<<"- .... "<<WorldMat;

    G4Box *solidWorld = new G4Box("World", Setup::world_hdx, Setup::world_hdy, Setup::world_hdz);
    logicWorld = new G4LogicalVolume(solidWorld, WorldMat, "World", 0, 0, 0);
    physiWorld = new G4PVPlacement(0, // no rotation
                                    G4ThreeVector(), // origin
                                    logicWorld, // its logical volume
                                    "World", // name
                                    0, // no mother log; null ptr
                                    false, // no boolean ops
                                    0); // copy

    logicWorld->SetVisAttributes(G4VisAttributes::Invisible);
    G4cout<< ".......... done! " << G4endl;
}

//// DESY 2016 Prototype test beam
void LCDetectorConstruction::BuildTBeamPT16(){
    // ==============================================================================================================
    // base units for 2016 TB  at desy : 
    //  1. absorber and sensor - "AS"  / "AS_PL", 1 mm gup, 1 slot, total =4.5mm
    //  2. absorber only base unit - "A" / "A_PL"  1 mm gup, 1 slot, total =4.5mm with Wabsorber_MGS (93% W )or Wabsorber_PL (95 % W) 
    //  3. For Tracker base unit with only sensor muodul and air gup  - "JSM:X",  total wide= 1 + X mm
    //  4. To add the plastic scintilator inftont use SC
    // ==============================================================================================================

    G4cout<<" Building Test Beam 2016..."<<G4endl;

    // x, y axes beam sizes: 
    G4double airhx = 150.0 *mm;
    G4double airhy = 150.0 *mm;

    // z axis beam size:
    //need to add z distences of elementts:
    // 0.5 mm for SensorModule and air gaps.
    // 2.25 mm for base unite with absorber.

    G4double airhz = hAbsorberPitchDZ + hTungstenDZ; // should be 2.25 mm - half a slot
    G4double airhz1mm = hAbsorberPitchDZ; // should be 0.5 mm 
    std::cout<<" pitchhz = "<<2*airhz*mm<<" mm "<<std::endl;
    std::cout<<" pitchhz1mm = "<<2*airhz1mm*mm<<" mm "<<std::endl;

    G4double airhz_PSC = 10.0*mm;
    G4double airhz_TR_PSC = 3.75*mm;

    // building air base units (6 of them) 
    std::cout << "-----------building air base units: "<<std::endl;
    G4Box *solidBaseUnit = new G4Box("solidBaseUnit", airhx, airhy, airhz);
    G4Box *solidBaseUnit_PL = new G4Box("solidBaseUnit_PL", airhx, airhy, airhz);
    G4LogicalVolume *logicBaseUnit = new G4LogicalVolume(solidBaseUnit, Air, "logicBaseUnit", 0, 0, 0); // 1
    G4LogicalVolume *logicBaseUnit_PL = new G4LogicalVolume(solidBaseUnit_PL, Air, "logicBaseUnit_PL", 0, 0, 0); // 2
    logicBaseUnit->SetVisAttributes(G4VisAttributes::Invisible);
    logicBaseUnit_PL->SetVisAttributes(G4VisAttributes::Invisible);

    G4Box *solidBaseUnitAB = new G4Box("solidBaseUnitAB", airhx, airhy, airhz);
    G4Box *solidBaseUnitAB_PL = new G4Box("solidBaseUnitAB_PL", airhx, airhy, airhz);  
    G4LogicalVolume *logicBaseUnitONLYAB = new G4LogicalVolume(solidBaseUnitAB, Air, "logicBaseUnitONLYAB", 0, 0, 0); // 3
    G4LogicalVolume *logicBaseUnitONLYAB_PL = new G4LogicalVolume(solidBaseUnitAB_PL, Air, "logicBaseUnitONLYAB_PL", 0, 0, 0); // 4
    logicBaseUnitONLYAB->SetVisAttributes(G4VisAttributes::Invisible);
    logicBaseUnitONLYAB_PL->SetVisAttributes(G4VisAttributes::Invisible);

    G4Box *solidBaseUnitFor1mm = new G4Box("solidBaseUnitFor1mm", airhx, airhy, airhz1mm);
    G4Box *solidBaseUnitFor1mm_PL = new G4Box("solidBaseUnitFor1mm_NoS", airhx, airhy,airhz1mm);  
    G4LogicalVolume *logicBaseUnitFor1mm = new G4LogicalVolume(solidBaseUnitFor1mm, Air, "logicBaseUnitFor1mm", 0, 0, 0); // 5
    G4LogicalVolume *logicBaseUnitFor1mm_NoS = new G4LogicalVolume(solidBaseUnitFor1mm_PL, Air, "logicBaseUnitFor1mm_NoS", 0, 0, 0); // 6
    logicBaseUnitFor1mm->SetVisAttributes(G4VisAttributes::Invisible);
    logicBaseUnitFor1mm_NoS->SetVisAttributes(G4VisAttributes::Invisible);
  
    G4Colour *SC_Color = new G4Colour(1., 0., 1., 1.);
    // for Plastic SC
    G4Box *solidBase_PSC = new G4Box("solidBase_PSC", airhx, airhy, airhz_PSC);
    G4LogicalVolume *logicBaseUnit_PSC = new G4LogicalVolume(solidBase_PSC, PLASTIC_SC , "logicBaseUnit_PSC", 0, 0, 0); // 5
    logicBaseUnit_PSC->SetVisAttributes(SC_Color);
    // for trigger SC
    G4Box *solidBase_TR_PSC = new G4Box("solidBase_TR_PSC", airhx, airhy, airhz_TR_PSC);
    G4LogicalVolume *logicBaseUnit_TR_PSC = new G4LogicalVolume(solidBase_TR_PSC, PLASTIC_SC , "logicBaseUnit_TR_PSC", 0, 0, 0); // 5
    logicBaseUnit_TR_PSC->SetVisAttributes(SC_Color);

    // add absorber (single X0) in front sensor 
    std::cout<<"-----------building absorber: "<<std::endl;

    G4double zposAbs0 = -airhz + hTungstenDZ;
    G4double zposAbs0_MSG = -airhz + hTungstenDZ*1.02;

    G4Box *solidAbs0 = new G4Box("solidAbs0", 70.*mm, 70.*mm, hTungstenDZ);
    G4Box *solidAbs0_MGS = new G4Box("solidAbs0_MGS", 70.*mm, 70.*mm, hTungstenDZ*1.02); // nominal thiknes of MSG plate is ~3.57
    G4Box *solidAbs0_PL = new G4Box("solidAbs0_PL", 70.*mm, 70.*mm, hTungstenDZ);

    G4LogicalVolume *logicAbs0 = new G4LogicalVolume(solidAbs0, Tungsten, "logicAbs0", 0, 0, 0);
    G4LogicalVolume *logicAbs0_MGS = new G4LogicalVolume(solidAbs0_MGS,Setup::Wabsorber_MGS , "logicAbs0_MGS", 0, 0, 0); // 2 kind of absorber plate we use in 2014 -2015 TB
    G4LogicalVolume *logicAbs0_PL = new G4LogicalVolume(solidAbs0_PL,Setup::Wabsorber_PL , "logicAbs0_PL", 0, 0, 0); 

    std::cout<<"-----------Placement absorber: "<<std::endl;

    new G4PVPlacement(0, G4ThreeVector(0., 0., zposAbs0_MSG), logicAbs0_MGS, "Absorber0_MGS", logicBaseUnit, false, 0, 1); // absorber in base unit 1 
    new G4PVPlacement(0, G4ThreeVector(0., 0., zposAbs0), logicAbs0_PL, "Absorber0_PL", logicBaseUnit_PL, false, 0, 1); // absorber in base unit 2 
    new G4PVPlacement(0, G4ThreeVector(0., 0., zposAbs0_MSG), logicAbs0_MGS, "Absorber0_MGS", logicBaseUnitONLYAB, false, 0, 1); // absorber in base unit 3 
    new G4PVPlacement(0, G4ThreeVector(0., 0., zposAbs0), logicAbs0_PL, "Absorber0_PL", logicBaseUnitONLYAB_PL, false, 0, 1); // absorber in base unit 4 

    G4VisAttributes *Abs0Att = new G4VisAttributes(G4Colour(0.0,0.0,1.0));
    
    logicAbs0->SetVisAttributes(Abs0Att);
    logicAbs0_MGS->SetVisAttributes(Abs0Att);
    logicAbs0_PL->SetVisAttributes(Abs0Att);  

    // carbon fiber suppurt 
    G4double CFhx = 70.0*mm;
    G4double CFhy = 70.0*mm;
    G4double CFhz = 0.395*mm;

    std::cout<<"-----------building carbon fiber support: "<<std::endl;

    G4Box *solidCF = new G4Box("solidCF", CFhx, CFhy, CFhz);
    G4LogicalVolume *logicCF = new G4LogicalVolume(solidCF, Setup::C_fiber, "logicCF", 0, 0, 0);
  
    G4VisAttributes *CF0Att = new G4VisAttributes(G4Colour(0.0, 0.0, 0.0));
    logicCF->SetVisAttributes(CF0Att);

    G4double sPhi = 75.*deg;
    G4double dPhi = 30.*deg;

    std::cout<<"-----------building sensor partrs: "<<std::endl;

    G4Tubs *solidSensV = new G4Tubs("solidSensorV", SensRadMin + deadPhi, SensRadMax - deadPhi, hSiliconDZ, sPhi, dPhi);
    G4Tubs *solidMetal = new G4Tubs("solidMetal", SensRadMin + deadPhi, SensRadMax - deadPhi, hMetalDZ, sPhi, dPhi);
    G4Tubs *solidFanoutFrnt = new G4Tubs("solidFanoutFrnt", SensRadMin, SensRadMax, hFanoutFrontDZ, sPhi, dPhi);
    G4Tubs *solidFanoutBack = new G4Tubs("solidFanoutBack", SensRadMin, SensRadMax, hFanoutBackDZ, sPhi, dPhi);

    logicSensorV = new G4LogicalVolume(solidSensV, Silicon, "logicSensorV", 0, 0, 0);
    logicMetalV = new G4LogicalVolume(solidMetal, Aluminium, "logicMetalV", 0, 0, 0);
    logicFanoutFrnt = new G4LogicalVolume(solidFanoutFrnt, FanoutMatF, "logicFanoutFront", 0, 0, 0); 
    logicFanoutBack = new G4LogicalVolume(solidFanoutBack, FanoutMatB, "logicFanoutFront", 0, 0, 0); 

    logicSensorV->SetVisAttributes(G4VisAttributes::Invisible);
    logicMetalV->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes *FanoutFrntAtt = new G4VisAttributes(G4Colour(1.0, 0.27, 0.0));
    logicFanoutFrnt->SetVisAttributes(FanoutFrntAtt);

    // placment of sensor part in CF

    G4double ypos = -(SensRadMin + 0.5*(SensRadMax-SensRadMin));
    
    std::cout<<"-----------placement sensor partrs in CF suppurt: "<<std::endl;

    std::cout<<"Some important sizes: "<<std::endl;
    std::cout<<"ypos            : "<<ypos<<std::endl;
    std::cout<<"CFhz            : "<<CFhz <<std::endl;
    std::cout<<"hFanoutFrontDZ  : "<<hFanoutFrontDZ<<std::endl;
    std::cout<<"hMetalDZ        : "<<hMetalDZ<<std::endl;
    std::cout<<"hSiliconDZ      : "<<hSiliconDZ<<std::endl;
    std::cout<<"hFanoutBackDZ   : "<<hFanoutBackDZ<<std::endl;
  
    G4double SensorAtCF = -CFhz + hFanoutFrontDZ; 

    std::cout<<"1.SensorAtCF: "<<SensorAtCF<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., ypos, SensorAtCF), logicFanoutFrnt, "FunOut0", logicCF, false, 0, 1);

    SensorAtCF = SensorAtCF + hFanoutFrontDZ + hMetalDZ;
    std::cout<<" 2.SensorAtCF: "<<SensorAtCF<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., ypos, SensorAtCF), logicMetalV, "PadMetal0", logicCF, false, 0, 1);
  
    SensorAtCF = SensorAtCF + hMetalDZ + hSiliconDZ; 
    std::cout<<"3.SensorAtCF: "<<SensorAtCF<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., ypos , SensorAtCF),logicSensorV , "SensorV0", logicCF, false, 0, 1);
    
    SensorAtCF = SensorAtCF + hSiliconDZ + hMetalDZ;
    std::cout<<"4.SensorAtCF: "<<SensorAtCF<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., ypos, SensorAtCF), logicMetalV , "PadMetal1", logicCF, false, 0, 1);
    
    SensorAtCF = SensorAtCF + hMetalDZ + hFanoutBackDZ;
    std::cout<<"5.SensorAtCF: "<<SensorAtCF<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., ypos , SensorAtCF), logicFanoutBack ,"FunOut1", logicCF, false, 0, 1);


    // placment of sensor & CF in base units 1,2,5
    std::cout<<"-----------placement of CF suppurt in air base units: "<<std::endl;
    new G4PVPlacement(0, G4ThreeVector(0., 0., airhz - CFhz ), logicCF, "CF0", logicBaseUnit, false, 0, 1); // 1
    new G4PVPlacement(0, G4ThreeVector(0., 0., airhz - CFhz ), logicCF, "CF0", logicBaseUnit_PL, false, 0, 1); // 2
    new G4PVPlacement(0, G4ThreeVector(0., 0., airhz1mm - CFhz ), logicCF, "CF0", logicBaseUnitFor1mm, false, 0, 1); // 5

    // put LCAL to world
  
    G4double zposLC = 3300.*mm; // for 2016 from 4th Telescope plane to box 
    G4double zyposLC = -(22. / 64.) * (SensRadMax-SensRadMin)*mm;
    G4double DUTairhx = 100.0*mm;
    G4double DUTairhy = 100.0*mm;
    G4double DUTairhz = 54*mm; // need to define that as Lcal_tungsten_hdz + C (spacer) 
    G4double DUTextrahz = 0.002*mm; 
    G4double ShiftForSigleMIP = 10*mm; 

    G4double zpos_PSC = 1130.*mm; // for 2016 from 4th Telescope plane to Plastic scintilator  
    G4double zpos_TR_PSC = 20.*mm; // for 2016 from 4th Telescope plane to TRiger Plastic scintilator  
    G4int n_layers = Setup::Lcal_n_layers;

    // for general use 
    std::cout<<"-----------N layers: "<<n_layers<<std::endl;

    int Is_SC = 0; 
    int Is_TSC = 0; 
    int Is_stag = 0; 
    G4double ypos_stag[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    std::istringstream iss((std::string)Setup::TBeam_senrio);
    G4int iplacelayer = 0; // for sensors 
    G4int iplaceElements = 0; // for all 
    std::string delimiter = ":";
    while(iss && (iplacelayer < n_layers)){
        std::stringstream placement_name("");
        std::string Osub;
        iss>>Osub;
        std::cout<<"Substring: "<<Osub<<std::endl;
        std::string delimiter = ":";
        std::string tmpsub = Osub.substr(0, Osub.find(delimiter));
        std::string sub;
        int i_air = 0;
        
        // check if the ":" for air gap size for tracker is there
        if(tmpsub.length() < Osub.length()){
            Osub.erase(0, 1 + tmpsub.length());
            i_air = std::atoi(Osub.c_str())*mm;
            sub = tmpsub;
        }
        else{
          sub = tmpsub;
        }
        
        if(sub == "AS"){
            placement_name<<"DUTAS"<<iplacelayer;
            new G4PVPlacement(0, G4ThreeVector(0., zyposLC+ypos_stag[iplacelayer], zposLC + airhz), logicBaseUnit, placement_name.str().c_str(), logicWorld, 0, iplacelayer+1, 1);
            G4cout<<"Placed "<<iplaceElements<<" as sensor number: "<<iplacelayer<<" at z = "<<zposLC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
            zposLC = zposLC + 2 * airhz + DUTextrahz;
            iplacelayer++;
        }
        else if(sub == "AS_PL"){
            placement_name<<"DUTASP"<<iplacelayer;
            new G4PVPlacement(0, G4ThreeVector(0., zyposLC+ypos_stag[iplacelayer], zposLC + airhz), logicBaseUnit_PL, placement_name.str().c_str(), logicWorld, 0, iplacelayer+1, 1);
            G4cout<<"Placed "<<iplaceElements<<" as sensor number: "<<iplacelayer<<" at z = "<<zposLC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
            zposLC = zposLC + 2 * airhz + DUTextrahz;           
            iplacelayer++;
        }       

        else if(sub == "A_PL"){
            placement_name<<"ABP"<<iplaceElements;
            new G4PVPlacement(0, G4ThreeVector(0., zyposLC, zposLC+airhz), logicBaseUnitONLYAB_PL, placement_name.str().c_str(), logicWorld, 0, iplaceElements +1, 1);
            G4cout<<"Placed "<<iplaceElements<<" at z = "<<zposLC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
            zposLC = zposLC + 2 * airhz + DUTextrahz;
        }
        
        else if(sub == "A"){
            placement_name<<"AB"<<iplaceElements;
            new G4PVPlacement(0, G4ThreeVector(0., zyposLC, zposLC + airhz), logicBaseUnitONLYAB, placement_name.str().c_str(), logicWorld, 0, iplaceElements+1, 1);
            G4cout<<"Placed "<<iplaceElements<<" at z = "<<zposLC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
            zposLC = zposLC + 2 * airhz + DUTextrahz;           
        }

        else if(sub == "JSM"){
            placement_name<<"DUTJSM"<<iplacelayer;
            new G4PVPlacement(0, G4ThreeVector(0., zyposLC+ypos_stag[iplacelayer], zposLC+ airhz1mm), logicBaseUnitFor1mm, placement_name.str().c_str(), logicWorld, 0, iplacelayer+1, 1);
            G4cout<<"Placed "<<iplaceElements<<" as sensor number : "<<iplacelayer<<" at z = "<<zposLC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
            zposLC = zposLC + 2 * airhz1mm + DUTextrahz + i_air; // add the air gap after 
            iplacelayer++;
        }
        
        else if(sub == "SC"){
            if(Is_SC == 0){
                placement_name<<"SC";
                new G4PVPlacement(0, G4ThreeVector(0., zyposLC, zpos_PSC), logicBaseUnit_PSC, placement_name.str().c_str(), logicWorld, 0, 1, 1);
                G4cout<<"Placed SC"<<" at z = "<<zpos_PSC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
                Is_SC = 1; // can be placed only 1 time  
            }
        }
        
        else if(sub == "TSC"){
            if(Is_TSC == 0){
                placement_name<<"TR_SC";
                new G4PVPlacement(0, G4ThreeVector(0., zyposLC, zpos_TR_PSC), logicBaseUnit_TR_PSC, placement_name.str().c_str(), logicWorld, 0, 1, 1);
                G4cout<<"Placed SC" <<" at z = "<<zpos_TR_PSC<<" layer with name "<<placement_name.str().c_str()<<G4endl;
                Is_TSC = 1; // can by placed only 1 time  
            }
        }
        
        else if(sub == "stag"){
            if(Is_stag == 0){
                G4cout<<"Stag should applay before all sensor layers "<<G4endl;
                double TB_stag[8] = {0.0, 0.0, 0.2*mm, -0.7*mm, 1.5*mm, -1.0*mm, 0.0 ,0.0}; // asked by Marina
                //double TB_stag[8] = {-0.11*mm, -1.26*mm, 0.46*mm, -0.275644*mm, 1.87705*mm, -1.2183*mm, 0.53323*mm, 0*mm}; // proper stagging
                //double TB_stag[8] = {0.11*mm, 1.26*mm, -0.46*mm, 0.275644*mm, -1.87705*mm, 1.2183*mm, -0.53323*mm, 0*mm};
                for(int istag = 0; istag < 8; istag++){
                    ypos_stag[istag] = TB_stag[istag];
                    G4cout<<"Stugging: layer "<<istag<<" movment: "<<ypos_stag[istag]<<G4endl;
                }
                Is_stag = 1; // can be placed only 1 time  
            }
        }

        else{
            G4cout<<"Substring: "<<sub<<" in layer "<<iplaceElements<<" is not recognized. we will break"<<G4endl;
            break;
        }
        iplaceElements++;
    }

    // SENSITIVE DETECTOR
    G4SDManager *SDman = G4SDManager::GetSDMpointer();

    // Initialize the Sensitive Detector
    SensDet = new LCSensitiveDetector("LumiCalSD", // name
                                      Cell0_Rad, // inner LC radius
                                      startPhi, // start angle
                                      CellPitch, // radial cell size
                                      sectorPhi, // angular cell width
                                      nCells, // # cells in the rad dir
                                      nSectors, // # cells in the phi dir
                                      VirtualCell); // cell type real/virtual = false/true

    // Cells are the sensitive detectors
    SDman->AddNewDetector(SensDet);
   
    if(VirtualCell) logicSensorV->SetSensitiveDetector(SensDet);
    else G4cout<<"There is no VirtualCell..."<<G4endl;

    G4cout<<"Test Beam setup is done!"<<G4endl;
}




void LCDetectorConstruction::BuildLCal()
{
    // LUMICAL DETECTOR
    // Contains all 30 layers
    G4cout<<"Processing LCAL volume..."<<G4endl;
    assert(FECave_rmin >= SensRadMax);

    G4double tolDZ = 0.010*mm;
    G4Tubs *solidWholeLC = new G4Tubs("solidWholeLC", innerRad, outerRad, hLumiCalDZ+tolDZ, startPhi, endPhi);
    logicWholeLC = new G4LogicalVolume(solidWholeLC, Air, "logicWholeLC", 0, 0, 0);
    G4cout<<"...finished."<<G4endl;

    // LAYER
    // Basic component of LumiCal
    // Contains sensors, absorbers, and fanout layers
    G4cout<<"Building Layer..."<<G4endl;

    G4Tubs *solidLayer = new G4Tubs("solidLayer", innerRad, SensRadMax, hLayerDZ, startPhi, endPhi);
    logicLayer = new G4LogicalVolume(solidLayer, Air, "logicLayer", 0, 0, 0);
    G4cout<<"...finished."<<G4endl;

    // ABSORBER
    // tungsten plate
    // goes in back layer

    if(Setup::Lcal_use_absorber){
        G4cout<<"Building Absorber..."<<G4endl;

        G4Tubs *solidAbsorber = new G4Tubs("solidAbsorber", innerRad, SensRadMax, hTungstenDZ, startPhi, endPhi);
        logicAbsorber = new G4LogicalVolume(solidAbsorber, Tungsten, "logicAbsorber", 0, 0, 0);
        G4cout<<"...finished."<<G4endl;
    }

    // FANOUT
    // Funout plates solids

    if(Setup::Lcal_use_fanout ){
        G4cout<<"Building Fanout..."<<G4endl;
        G4Tubs *solidFanoutBack = new G4Tubs("solidFanoutBack", innerRad, SensRadMax, hFanoutBackDZ, startPhi, endPhi);
        G4Tubs *solidFanoutFrnt = new G4Tubs("solidFanoutBack", innerRad, SensRadMax, hFanoutFrontDZ, startPhi, endPhi);
        logicFanoutFrnt = new G4LogicalVolume(solidFanoutFrnt, FanoutMatF, "logicFanoutFront", 0, 0, 0);  
        logicFanoutBack = new G4LogicalVolume(solidFanoutBack, FanoutMatB, "logicFanoutBack", 0, 0, 0); 
        G4cout<<"...finished."<<G4endl;
    }

    // Lumical support
    // EARS
    if(Setup::Lcal_support){
        G4double MechSpaceRmax = SensRadMax + Lcal_extra_size;
        G4double EarsBaseRmax = SensRadMax + 2.*mm;
        G4double ear_dx = SensRadMax + Setup::Lcal_space_for_ears;
        G4double ear_dy = 10.*ear_dx/sqrt(ear_dx*ear_dx-SensRadMax*SensRadMax);
        G4double ear_hdz = (hLayerDZ - FECave_hDZ);
        G4double ear_z = hLayerDZ - ear_hdz;
        G4int nBolts = 3;
        G4double bolt_r = 4.*mm;
        G4double xhole = EarsBaseRmax + Setup::Lcal_space_for_ears/2;

        G4Tubs *solidMechSpace = new G4Tubs("solidMech", SensRadMax, MechSpaceRmax, hLumiCalDZ, startPhi, endPhi); 
        G4Tubs *solidMechLay = new G4Tubs("solidMechLay", SensRadMax, MechSpaceRmax, hLayerDZ, startPhi, endPhi);
        G4Tubs *solidEarsBase = new G4Tubs("solidEarsBase", SensRadMax, EarsBaseRmax, ear_hdz, startPhi, endPhi); 
        G4Tubs *solidBolt = new G4Tubs("solidBolt", 0., bolt_r, hLayerDZ, startPhi, endPhi);
 
        G4LogicalVolume *logicMechSpace = new G4LogicalVolume(solidMechSpace, Air, "logicMechSpace", 0, 0, 0); 
        G4LogicalVolume *logicMechLay = new G4LogicalVolume(solidMechLay, Air, "logicMechLay", 0, 0, 0); 
        G4LogicalVolume *logicEarsBase = new G4LogicalVolume(solidEarsBase, Tungsten, "logicEarsBase", 0, 0, 0); 
        G4LogicalVolume *logicBolt = new G4LogicalVolume(solidBolt, Iron, "logicBolt", 0, 0, 0); 

        new G4PVDivision("MechLayer", logicMechLay, logicMechSpace, kZAxis, nLayers, 0);
        new G4PVPlacement(0, G4ThreeVector(0., 0., ear_z), logicEarsBase, "EarsBase", logicMechLay, false, 0); 

        // make an "ear"

        G4EllipticalTube *solidEar0 = new G4EllipticalTube("solidEar0", ear_dx, ear_dy, ear_hdz);
        G4Tubs *clipper = new G4Tubs("clipper", 0., EarsBaseRmax, hLayerDZ, startPhi, endPhi);

        G4SubtractionSolid *solidEar1 = new G4SubtractionSolid("solidEar1", solidEar0, clipper, 0, G4ThreeVector(0., 0., 0.));
        // punch a hole for the bolt
        G4Tubs *puncher = new G4Tubs("puncher", 0., bolt_r, hLayerDZ, startPhi, endPhi);
        // final "ear"
        G4SubtractionSolid *solidEar2 = new G4SubtractionSolid("solidEar2", solidEar1, puncher, 0, G4ThreeVector(xhole, 0., 0.));
        G4SubtractionSolid *solidEar = new G4SubtractionSolid("solidEar", solidEar2, puncher, 0, G4ThreeVector(-xhole, 0., 0.));
        // final logical
        G4LogicalVolume *logicEar = new G4LogicalVolume(solidEar, Tungsten, "logicEar", 0, 0, 0);
        // placement 
        // populate "MechLayer" with "Ear"s and Bolts
        G4double dphi_rot = endPhi / G4double(2 * nBolts);
        G4double supAng = 0.;
        G4double ear_phi = supAng;
        for(int i=0 ;i < nBolts; i++){
            std::stringstream strlayer;
            strlayer<<i+1;
            G4String EarName = G4String("Ear") + G4String(strlayer.str());
       
            G4Transform3D transear(G4RotationMatrix().rotateZ(ear_phi), G4ThreeVector(0., 0., ear_z).rotateZ(ear_phi));
            G4Transform3D tranbolt1(G4RotationMatrix().rotateZ(ear_phi), G4ThreeVector(xhole, 0., 0.).rotateZ(ear_phi));
            G4Transform3D tranbolt2(G4RotationMatrix().rotateZ(ear_phi + 180.*deg), G4ThreeVector(xhole, 0., 0.).rotateZ(ear_phi + 180.*deg));
 
            new G4PVPlacement(transear, logicEar, EarName, logicMechLay, false, i+1);
            G4String BoltNam = G4String("Bolt") + G4String(strlayer.str());
            new G4PVPlacement(tranbolt1, logicBolt, BoltNam, logicMechLay, false, i+1);
            strlayer<<i+5;
            BoltNam = G4String("Bolt") + G4String(strlayer.str());
            new G4PVPlacement(tranbolt2, logicBolt, BoltNam, logicMechLay, false, i+5);
            ear_phi += dphi_rot;
        }

        // sandwich the sensor layers. Go inside layer
        G4double FErmax = FECave_rmax - 4.*mm;
        G4double FErmin = FECave_rmin + 3.*mm;
        G4double d_Gap = 10.*mm;
        G4double FE_phi1 = atan(d_Gap/FECave_rmin);
        G4double FE_dphi = endPhi/G4double(2*nBolts) - 2.*FE_phi1;

        // Front-End mother space solid
        G4Tubs *solidFEmother = new G4Tubs("solidFE", FECave_rmin, FECave_rmax, FECave_hDZ, FE_phi1, FE_dphi);
        G4Tubs *ChipEnv = new G4Tubs("ChipEnv" , FECave_rmin, FECave_rmax, FEChip_hDZ, FE_phi1, FE_dphi);
        G4Tubs *solidPCB = new G4Tubs("solidPCB", FECave_rmin, FECave_rmax, PCB_hDZ, FE_phi1, FE_dphi);

        logicFEmother = new G4LogicalVolume(solidFEmother, Air, "logicFEmother", 0, 0, 0);
        G4LogicalVolume *logicChipEnv = new G4LogicalVolume(ChipEnv, Air, "logicChipEnv", 0, 0, 0);
        logicPCB = new G4LogicalVolume(solidPCB, FanoutMatB, "logicPCB", 0, 0, 0);

        // Front-End chips
        G4int nFE_Sectors = nSectors/(2*nBolts);
        G4double FE_Sec_dphi = FE_dphi/G4double(nFE_Sectors);
        G4Tubs *solidFESector = new G4Tubs("solidFE", FErmin, FErmax, FEChip_hDZ, FE_phi1, FE_Sec_dphi);
        logicFESector = new G4LogicalVolume(solidFESector, Air, "logicFESector", 0, 0, 0);

        new G4PVDivision("FE-sector", logicFESector, logicChipEnv, kPhi, nFE_Sectors, 0);

        // a chip
        G4double hx = (FErmax - FErmin)/2.;
        G4double hy = FErmin*tan( FE_Sec_dphi/2.) - 1.*mm;
        G4double hz = FEChip_hDZ;
        G4Box *solidChip = new G4Box("solidChip", hx, hy, hz);
        logicChip = new G4LogicalVolume(solidChip, Silicon, "logicFEChip", 0, 0, 0);

        // populate FEmother with chips
        G4ThreeVector xFE = G4ThreeVector((FErmin+FErmax)/2., 0., 0.);
        G4Transform3D transFE(G4RotationMatrix().rotateZ((FE_phi1+FE_Sec_dphi/2.)), xFE.rotateZ((FE_phi1 + FE_Sec_dphi/2.)));
        new G4PVPlacement(transFE, logicChip, "FE-chip", logicFESector, false, 0);
        G4double Zpos = FECave_hDZ - PCB_hDZ;
        new G4PVPlacement(0, G4ThreeVector(0., 0., Zpos), logicPCB, "LcalPCB", logicFEmother, false, 0);
        Zpos = FECave_hDZ - 2. * PCB_hDZ - FEChip_hDZ;
        new G4PVPlacement ( 0, G4ThreeVector(0.,0.,Zpos), logicChipEnv, "FE-Set", logicFEmother, false, 0);

      G4VisAttributes *PCBVisAtt = new G4VisAttributes(G4Colour(0.0, 0.5, 0.0));
      logicPCB->SetVisAttributes(PCBVisAtt);
      logicFEmother->SetVisAttributes( G4VisAttributes::Invisible );
      logicChipEnv->SetVisAttributes( G4VisAttributes::Invisible );
      logicFESector->SetVisAttributes( G4VisAttributes::Invisible );

   //-------------------------------
   // put Front-End into mech layer
   //-------------------------------     
    if( Setup::Lcal_use_FE ) {
      G4double FE_z = hLayerDZ - 2.*ear_hdz - FECave_hDZ;
      ear_phi = supAng;
      for ( int k=0; k< nBolts ; k++ ){

    G4Transform3D FErot1 ( G4RotationMatrix().rotateZ( ear_phi ), G4ThreeVector( 0., 0., FE_z).rotateZ( ear_phi ));
    G4Transform3D FErot2 ( G4RotationMatrix().rotateZ( ear_phi +180.*deg), G4ThreeVector( 0., 0., FE_z).rotateZ( ear_phi +180.*deg));
    new G4PVPlacement ( FErot1 , logicFEmother, "FrontEndChips", logicMechLay, false, k);
    new G4PVPlacement ( FErot2 , logicFEmother, "FrontEndChips", logicMechLay, false, k+nBolts );
        ear_phi += dphi_rot;
      }
    }
      // put mechanics into LCAL
 
      new G4PVPlacement ( 0, G4ThreeVector(0.,0.,-tolDZ), logicMechSpace, "LcalSupport", logicWholeLC, false, 1);
      //
      // visual attributes
      //
       G4VisAttributes *FanoutVisAtt = new G4VisAttributes(G4Colour(0.0, 0.7, 0.0));
       G4VisAttributes *FEVisAtt = new G4VisAttributes(G4Colour(0.3, 0.2, 0.5));
       G4VisAttributes *AbsorberVisAtt = new G4VisAttributes(G4Colour(0.7, 0.0, 0.7));
       G4VisAttributes *IronVisAtt     = new G4VisAttributes(G4Colour(0.2, 0.3, 0.7));
       FEVisAtt->SetForceWireframe(true);
     logicEarsBase->SetVisAttributes(AbsorberVisAtt);
     logicEar->SetVisAttributes(AbsorberVisAtt);
     logicBolt->SetVisAttributes(IronVisAtt);
     logicMechLay ->SetVisAttributes(G4VisAttributes::Invisible); 
     logicMechSpace ->SetVisAttributes(G4VisAttributes::Invisible);      
         logicFanoutFrnt->SetVisAttributes(FanoutVisAtt);
         logicFanoutBack->SetVisAttributes(FanoutVisAtt);
         logicChip->SetVisAttributes(FEVisAtt);
   

          }// endif Lcal_support


    //---------------
    // SENSOR
    //---------------
    // contains sectors and cells
    // move this into Layer when finished
          G4cout << " Building sensor  ....."<< G4endl;
   G4Tubs *solidSensor0 = new G4Tubs("Sensortmp", 0.,SensRadMax, hSensorDZ, startPhi, endPhi);
   G4Tubs *solidSensor1 = new G4Tubs("SensortSi", 0.,SensRadMax, hSiliconDZ, startPhi, endPhi);
   G4double rcorner[2], zcorner[2];
   G4double r0[2] = {0., 0.};
   rcorner[0] = SensRadMin*cos(tilePhi/2.);
   rcorner[1] = rcorner[0];
   zcorner[0] = -2.*hSiliconDZ;
   zcorner[1] =  2.*hSiliconDZ;
   G4Polyhedra *puncher0 = new G4Polyhedra("puncher0",
                       startPhi - Setup::Lcal_Phi_Offset,
                       endPhi, nTiles, 2, zcorner, r0, rcorner);
   //
   G4double dphiRot = endPhi/G4double( nTiles );
   G4double phiRot = -Setup::Lcal_Phi_Offset;
   // final solid sensor mother volume
   G4SubtractionSolid *solidSensorEnv = new G4SubtractionSolid("solidSensor" , solidSensor0, puncher0, 0, G4ThreeVector( 0., 0., 0.));
   G4SubtractionSolid *solidSensorSi = new G4SubtractionSolid("solidSensor" , solidSensor1, puncher0, 0, G4ThreeVector( 0., 0., 0.));
   // and logical volume
     logicSensorEnv = new G4LogicalVolume(solidSensorEnv, Air, "logicSensorEnv", 0,0,0); 
     logicSensor = new G4LogicalVolume(solidSensorSi, Silicon, "logicSensor", 0,0,0);
     new G4PVPlacement ( 0, G4ThreeVector( 0.,0.,hSensorDZ - hSiliconDZ), logicSensor, "SiliconWafer", logicSensorEnv, false,0);
   
   
   
   if ( VirtualCell ) {

     G4Tubs *solidSensV = new G4Tubs("solidSensorV", SensRadMin + deadPhi ,SensRadMax - deadPhi, hSiliconDZ, startPhi, endPhi);
     G4Tubs *solidMetal = new G4Tubs("solidMetal", SensRadMin + deadPhi ,SensRadMax - deadPhi, hMetalDZ, startPhi, endPhi);
     logicSensorV = new G4LogicalVolume(solidSensV, Silicon, "logicSensorV", 0,0,0);
     logicMetalV = new G4LogicalVolume(solidMetal, Aluminium, "logicMetalV", 0,0,0);
     new G4PVPlacement ( 0, G4ThreeVector( 0.,0.,0.), logicSensorV, "SensorV", logicSensor, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0.,0.,-hSensorDZ+hMetalDZ), logicMetalV, "PadMetal", logicSensorEnv, false, 0);
     //
     // dead gap
     //
     if ( deadPhi > 0. ){
       G4double dx = (SensRadMax - SensRadMin - 2.*deadPhi);
       G4double hdy = deadPhi + 0.05 *mm;
       G4double sn  = sin ( hdy/dx ); 
       G4double hdx = sqrt( 1. - sn*sn )*dx/2.;  // must be a bit shorter not to extend outside sensor 
     G4Box *solidAirGap1     = new G4Box("dAirGap", hdx, 0.05*mm, hSiliconDZ );
     G4Box *solidDeadGap1    = new G4Box("DeadGap", hdx, deadPhi + 0.05*mm, hSiliconDZ );
     G4Box *solidAirGap2     = new G4Box("dAirGap", hdx, deadPhi/2.+0.05*mm, hMetalDZ );
     G4Box *solidDeadGap2    = new G4Box("DeadGap", hdx, deadPhi  + 0.05*mm, hMetalDZ );
     G4LogicalVolume *logicAirGap1 = new G4LogicalVolume( solidAirGap1, Air, "logAirGap1", 0,0,0);
     G4LogicalVolume *logicAirGap2 = new G4LogicalVolume( solidAirGap2, Air, "logAirGap2", 0,0,0);
     G4LogicalVolume *logicDeadGap1 = new G4LogicalVolume( solidDeadGap1, Silicon, "logDeadGap1", 0,0,0);
     G4LogicalVolume *logicDeadGap2 = new G4LogicalVolume( solidDeadGap2, Aluminium, "logDeadGap2", 0,0,0);
     //
     new G4PVPlacement ( 0, G4ThreeVector( 0.,0.,0.), logicAirGap1, "AirGap1", logicDeadGap1, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0.,0.,0.), logicAirGap2, "AirGap2", logicDeadGap2, false, 0);
     //
     //
     G4double xpos = (SensRadMin + SensRadMax)/2.;
     
     for ( G4int it = 0; it < nTiles; it++) {
       //      G4cout << it << " phi "<< phiRot /deg << G4endl;
      G4Transform3D zrot1 ( G4RotationMatrix().rotateZ( phiRot ), G4ThreeVector( xpos, 0., 0.).rotateZ( phiRot ));
     new G4PVPlacement( zrot1, logicDeadGap1, "deadGap1", logicSensorV, false, it+1);
     new G4PVPlacement( zrot1, logicDeadGap2, "deadGap2", logicMetalV, false, it+1);     
       phiRot += dphiRot;
     }
     }
   }

   if ( !VirtualCell ) {
   //---------------
    // SECTORS
    //---------------
    // we have three types of sectors 
    //  1. first in a tile - dead area at startPhi ( LCCellParam1 )
    //  2  second and third in a tile - no dead area ( regular replicating )
    //  3  forth in a tile - dead are at endPhi ( LCCellParam2 )
    //  Contains cells

    G4Tubs *solidSector = new G4Tubs("solidSector",
                                 SensRadMin,     // LumiCal rad
                                 SensRadMax,     // LumiCal rad
                                 hSiliconDZ,    // same as cell thickness
                 startPhi - Setup::Lcal_Phi_Offset,
                                 sectorPhi);   // width of a sector, 7.5deg
    G4Tubs *solidMetSec = new G4Tubs("solidMetSec",
                                 SensRadMin,     // LumiCal rad
                                 SensRadMax,     // LumiCal rad
                                 hMetalDZ,    // same as cell thickness
                                 startPhi- Setup::Lcal_Phi_Offset,
                                 sectorPhi);   // width of a sector, 7.5deg

    MetSector1 = new G4LogicalVolume(solidMetSec, Air, "MetSector1", 0, 0, 0);
    MetSector2 = new G4LogicalVolume(solidMetSec, Air, "MetSector2", 0, 0, 0);
    MetSector4 = new G4LogicalVolume(solidMetSec, Air, "MetSector4", 0, 0, 0);

    logicSector1 = new G4LogicalVolume(solidSector, Silicon, "logicSector1", 0, 0, 0);
    logicSector2 = new G4LogicalVolume(solidSector, Silicon, "logicSector2", 0, 0, 0);
    logicSector4 = new G4LogicalVolume(solidSector, Silicon, "logicSector4", 0, 0, 0);
    //
    // populate sensor with sectors replica
    //
         G4String SectorName;
     G4ThreeVector z0( 0., 0., hSensorDZ - hSiliconDZ);
     G4ThreeVector zm( 0., 0.,-hSensorDZ + hMetalDZ);
         G4int sec_per_tile = nSectors/nTiles;
         assert ( nSectors%nTiles == 0 );
     G4int SectorNum = 0;
     //  G4double phiFix = Setup::Lcal_Phi_Offset;
     G4double phiFix = 0.;
    for ( int itile=0; itile < nTiles; itile++){
      SectorNum++;
      G4RotationMatrix *zrot1 = new G4RotationMatrix();
      zrot1->rotateZ( phiFix - sectorPhi*((G4double)SectorNum - 1.) );      
      std::stringstream strsec1;
      strsec1 << SectorNum;
      SectorName = G4String("Sector") + G4String(strsec1.str());
      new G4PVPlacement( zrot1, z0, logicSector1, SectorName, logicSensor, false, SectorNum);
      SectorName = G4String("MetSect") + G4String(strsec1.str());
      new G4PVPlacement( zrot1, zm, MetSector1, SectorName, logicSensor, false, SectorNum);
      for ( int nsec = 2; nsec < sec_per_tile; nsec++ ) {
    SectorNum++;
    G4RotationMatrix *zrot2 = new G4RotationMatrix();
        zrot2->rotateZ( phiFix - sectorPhi*((G4double)SectorNum - 1.));
        std::stringstream strsec;
        strsec << SectorNum;
        SectorName = G4String("Sector") + G4String(strsec.str());
        new G4PVPlacement( zrot2, z0, logicSector2, SectorName, logicSensor, false, SectorNum);
       SectorName = G4String("MetSect") + G4String(strsec.str());
       new G4PVPlacement( zrot2, zm, MetSector2, SectorName, logicSensor, false, SectorNum);
      }
    SectorNum++;
    std::stringstream strsec4;
    strsec4 << SectorNum;
    SectorName = G4String("Sector") + G4String(strsec4.str());
    G4RotationMatrix *zrot4 = new G4RotationMatrix();  
    zrot4->rotateZ( phiFix - sectorPhi*((G4double)SectorNum - 1.) );
    new G4PVPlacement( zrot4, z0, logicSector4, SectorName, logicSensor, false, SectorNum);
        SectorName = G4String("MetSect") + G4String(strsec4.str());
        new G4PVPlacement( zrot4, zm, MetSector4, SectorName, logicSensor, false, SectorNum);

    }

   //---------------
    // CELL
    //---------------
    // Replicate inside Sector volumes
    // Parameterized to maintain constant separation in radial and phi dir.
    // Sensitive
    G4Tubs *solidCell = new G4Tubs("solidCell",
                           81.3 *mm,           //<----
                           83.0 *mm,           // all dummy arguments the actual ones
               0.160 *mm,          // will be computed by CellParam
                           0.0,                 // 
                           7.5*deg);           //<------
    G4Tubs *solidCellMet = new G4Tubs("solidCellMet",
                           81.3 *mm,           //<----
                           83.0 *mm,           // all dummy arguments the actual ones
               0.01 *mm,          // will be computed by CellParam
                           0.0,                 // 
                           7.5*deg);           //<------
    logicCell = new G4LogicalVolume(solidCell, Silicon, "logicCell", 0, 0, 0);
    logicCellMet = new G4LogicalVolume(solidCellMet, Aluminium, "logicCellMet", 0, 0, 0);

    LCCellParam *CellParam = new LCCellParam(nCells,                             // # cells/sector
                                             SensRadMin,                         // LC inner rad
                                             SensRadMax,                         // LC outer rad
                         hSensorDZ,
                                             hSiliconDZ,                         // Si thickness
                                             hMetalDZ,                          // pad metal thickness
                                             deadPhi,                            // clipSize in phi
                                             startPhi - Setup::Lcal_Phi_Offset,  // start angle
                                             sectorPhi);                         // sector angle

    // Replicate cells:
    new G4PVParameterised("Cell", logicCell, logicSector1,
                          kZAxis, nCells, CellParam);

    new G4PVParameterised("Cell", logicCell, logicSector2,
                          kZAxis, nCells, CellParam);
                          
    new G4PVParameterised("Cell", logicCell, logicSector4,
                          kZAxis, nCells, CellParam);
    // cell metalization
    new G4PVParameterised("CellMet", logicCellMet, MetSector1,
                          kZAxis, nCells, CellParam);

    new G4PVParameterised("CellMet", logicCellMet, MetSector2,
                          kZAxis, nCells, CellParam);
                          
    new G4PVParameterised("CellMet", logicCellMet, MetSector4,
                          kZAxis, nCells, CellParam);


   } // end if !VirtualCell
    G4cout << "                    ...done.    " << G4endl;

    //
    //
    G4cout << " Assembling Layer ....."<< G4endl;
    G4int ordnum = 1;
    //---------------
    // TUNGSTEN ABSORBER
    //---------------
    // 
    G4double absorberZ = hLayerDZ - hTungstenDZ;

    if ( Setup::Lcal_use_absorber ){

      G4cout << "               " << ordnum << ". Tungsten Absorber " << G4endl;   
                    new G4PVPlacement(0,
                                      G4ThreeVector(0, 0, absorberZ),
                                      logicAbsorber,
                                      "Absorber",
                                      logicLayer,
                                      false,
                                      0);
            ordnum++;
    }

   //---------------
    // FANOUTBACK
    //---------------
    // 
    G4double fanoutbackZ = absorberZ - hTungstenDZ - hFanoutBackDZ;
    
    if ( Setup::Lcal_use_fanout ) {
      G4cout << "               " << ordnum << ". FanOut back " << G4endl;

      new G4PVPlacement(0, G4ThreeVector(0, 0, fanoutbackZ), logicFanoutBack, "FanoutBack", logicLayer, false, 0);
      ordnum++;
    }
 
    //---------------
    // SENSOR
    //--------------- 
      G4cout << "               " << ordnum << ". Sensor plane " << G4endl;
      G4double sensorZ = fanoutbackZ - hFanoutBackDZ - hSensorDZ;
      SensZ0  = 2.*( hLayerDZ - hTungstenDZ - hFanoutBackDZ ) - hSiliconDZ;

    new G4PVPlacement(0, G4ThreeVector(0, 0, sensorZ), logicSensorEnv, "LcalSensor", logicLayer, false, 0);
    ordnum++;

    //---------------
    // FANOUTFRONT
    //---------------
    G4double fanoutfrntZ = sensorZ - hSensorDZ  - hFanoutFrontDZ;

    if ( Setup::Lcal_use_fanout ) {
      G4cout << "               " << ordnum << ". FanOut front " << G4endl;
   
      new G4PVPlacement(0, G4ThreeVector(0, 0, fanoutfrntZ), logicFanoutFrnt, "FanoutFront", logicLayer, false, 0);
      }


    G4cout << " Placing layers in LCAL .... "<< G4endl;
    //---------------
    // LAYERS PLACEMENT
    //---------------

    // Rotate every odd layer by 1/2 the sector angle
    // Rotation matrix
    // Thanks Bogdan
    // First layer position
    G4double placeLayer = -(hLumiCalDZ+tolDZ) + hLayerDZ;
    G4double PhiRot = -Setup::Lcal_layers_phi_offset;
    if ( !Setup::Lcal_layer_fan ){
      G4RotationMatrix *layerRotation = new G4RotationMatrix;
      layerRotation->rotateZ( PhiRot );
    // Place layers into LumiCal
      for (int i = 0; i < nLayers; i++) {
        // Keep track of the names of the different layers
        std::stringstream strlayer;
    strlayer << i+1;
        G4String LayerName = G4String("Layer") + G4String(strlayer.str());
        if ((i+1)%2) { // odd do rotation!
            new G4PVPlacement(layerRotation,
                              G4ThreeVector(0, 0, placeLayer),
                              logicLayer,
                              LayerName, // an updated string
                              logicWholeLC,
                              0,
                              i+1); // copy number
        } else {
            new G4PVPlacement(0,
                              G4ThreeVector(0, 0, placeLayer),
                              logicLayer,
                              LayerName, // an updated string
                              logicWholeLC,
                              0,
                              i+1); // copy number
        }

    //  G4cout << LayerName << " z-pos = " << placeLayer / mm << " [mm]"<< G4endl;
    
        // update the placecement for the next layer
        placeLayer += 2.*hLayerDZ;
      }
    }else{
      // layers "fanlike" configuration
      G4double rotang = 0.;
      for (int i = 0; i < nLayers; i++) {
        //
    G4RotationMatrix *layerRotation = new G4RotationMatrix;
    layerRotation->rotateZ( rotang );
        std::stringstream strlayer;
        strlayer << i+1;
        G4String LayerName = G4String("Layer") + G4String(strlayer.str());
    //
            new G4PVPlacement(layerRotation,
                              G4ThreeVector(0, 0, placeLayer),
                              logicLayer,
                              LayerName, 
                              logicWholeLC,
                              0,
                              i+1); 
     //
    rotang += PhiRot;
        placeLayer += 2.*hLayerDZ;
      }

    }
          G4cout << "                   ...LCAL  done!    " << G4endl;
 
    //
    //------------------------------------------------------------------------------
    //
    //  whole LCAL placement
    //
    Setup::Lcal_sens_Z0 = Lcal_zbegin + SensZ0; 

    G4double zpos = Lcal_zbegin + hLumiCalDZ + tolDZ;

    G4Transform3D trans1( G4RotationMatrix().rotateY(rotAng1),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng1));
    G4Transform3D trans2( G4RotationMatrix().rotateY(rotAng2),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng2));

                  new G4PVPlacement( trans1 ,
                                     logicWholeLC,
                                     "LumiCalDetector1",
                                     logicWorld,
                                     false,
                                     1);
                  new G4PVPlacement( trans2,
                                     logicWholeLC,
                                     "LumiCalDetector2",
                                     logicWorld,
                                     false,
                                     2);
    //  LCAL region 
    regionLCal = new G4Region("LCAL");
    logicWholeLC->SetRegion(regionLCal);
    regionLCal->AddRootLogicalVolume( logicWholeLC );

    //---------------
    // SENSITIVE DETECTOR
    //---------------
    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    // Initialize the Sensitive Detector
    SensDet = new LCSensitiveDetector("LumiCalSD",  // name
                                      Cell0_Rad,    // inner LC radius
                                      startPhi,     // start angle
                                      CellPitch,    // radial cell size
                                      sectorPhi,    // angular cell width
                                      nCells,       // # cells in the rad dir
                                      nSectors,     // # cells in the phi dir
                      VirtualCell); // cell type real/virtual =  false/true
        
    SDman->AddNewDetector(SensDet);
    // the Cells are the sensitive detectors
    G4cout << " Make logicCell sensitive detector .... " << G4endl;

    if ( !VirtualCell )  logicCell->SetSensitiveDetector(SensDet);
    else logicSensorV->SetSensitiveDetector( SensDet );

    G4cout << "                                .... done! " << G4endl;
 
    //----------------------------------------
    //--------------- LCAL VISUALIZATION ATTRIBUTES
    //----------------------------------------


  
    //  whole LCAL 
     G4VisAttributes *WholeLCVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
     //         logicWholeLC->SetVisAttributes(G4VisAttributes::Invisible);
     //         WholeLCVisAtt->SetDaughtersInvisible(true);
     WholeLCVisAtt->SetForceWireframe ( true );
         logicWholeLC->SetVisAttributes(WholeLCVisAtt);
       

     G4VisAttributes *LayerVisAtt = new G4VisAttributes(false, G4Colour(1.0, 1.0, 1.0));
         LayerVisAtt->SetForceWireframe (true);
         logicLayer->SetVisAttributes(LayerVisAtt);

    
     G4VisAttributes *AbsorberVisAtt = new G4VisAttributes(G4Colour(0.7, 0.0, 0.7));
     G4VisAttributes *IronVisAtt     = new G4VisAttributes(G4Colour(0.2, 0.3, 0.7));
     if(Setup::Lcal_VisAbsSolid ){
       AbsorberVisAtt->SetForceSolid(true);
       IronVisAtt->SetForceSolid(true);
     }
     else {
       AbsorberVisAtt->SetForceWireframe(true);
       IronVisAtt->SetForceWireframe(true);
     }
     if ( Setup::Lcal_use_absorber ) logicAbsorber->SetVisAttributes(AbsorberVisAtt);

       G4VisAttributes *SensorVisAtt = new G4VisAttributes(G4Colour(1., 0., 0.));
    if ( Setup::Lcal_VisSensSolid ) SensorVisAtt->SetForceSolid(true);
    else SensorVisAtt->SetForceWireframe(true);
    logicSensor->SetVisAttributes(SensorVisAtt);

    G4VisAttributes *SectorVisAtt = new G4VisAttributes(G4Colour(1. , 0., 0.));
    G4VisAttributes *CellVisAtt = new G4VisAttributes(true, G4Colour(0.6, 0.3, 0.02));
    if ( !VirtualCell ){
    SectorVisAtt->SetDaughtersInvisible(true);
    SectorVisAtt->SetForceWireframe(true);
    logicSector1->SetVisAttributes( SectorVisAtt );
    logicSector2->SetVisAttributes( SectorVisAtt );
    logicSector4->SetVisAttributes( SectorVisAtt );
    //
    MetSector1->SetVisAttributes(G4VisAttributes::Invisible);
    MetSector2->SetVisAttributes(G4VisAttributes::Invisible);
    MetSector4->SetVisAttributes(G4VisAttributes::Invisible);
 
    CellVisAtt->SetForceWireframe(true);
    logicCell->SetVisAttributes(CellVisAtt);
    }else{
      logicSensorV->SetVisAttributes(CellVisAtt);
    }


    G4cout << " Mass of the LCAL: " <<   logicWholeLC->GetMass()/ kg << G4endl;

    //--------------- DONE BUILDING!
}




void LCDetectorConstruction::BuildField()
{
    //--------------------------------------------------
    //                Magnetic Field
    //--------------------------------------------------
  G4cout<< "LCDetectorConstrucion::Construct(): Building Field ...."<< G4endl;;
    static G4bool fieldIsInitialized = false;
  if(!fieldIsInitialized)
  {
    G4FieldManager* fieldMgr
      = G4TransportationManager::GetTransportationManager()
        ->GetFieldManager();
    if( Setup::Beam_Crossing_Angle == 0. ) {
      LCField* myField = new LCField();

      fieldMgr->SetDetectorField(myField);
      fieldMgr->CreateChordFinder(myField);
              }else{
      G4cout << "LCDDetectorConstructor:: Using DID field map "<<G4endl;
          LCFieldX14* myField = new LCFieldX14("did_field.data"); // anti-DID field map for 14mr 
         fieldMgr->SetDetectorField(myField);
         fieldMgr->CreateChordFinder(myField);
       }
    fieldIsInitialized = true;
  }
     G4cout<< "             Field done. " << G4endl;
}

void LCDetectorConstruction::BuildBeamPipe()
{

     //-----------------------------------------
    //---    Beam pipe 
    //-----------------------------------------

     //
     //  beam pipe vis attributes 
     //  
    G4VisAttributes *BeamPipeVisAtt = new G4VisAttributes(G4Colour(0.26,0.33,0.41));
    G4VisAttributes *VacuumVisAtt = new G4VisAttributes(false); // just vacuum nothing to see
    if ( Setup::Beam_pipe_VisSolid ) BeamPipeVisAtt->SetForceSolid(true);
    else BeamPipeVisAtt->SetForceWireframe(true);
 
    G4cout << " Building BeamPipe ..." << G4endl;
     G4double LcalToBeamTol = 1.*mm;
         G4double zpl[6], rinner[6], router[6];
         G4double dzTol = 10. *mm;
     G4int numz = 6 ;

       zpl[0] = -Lcal_zbegin + dzTol ; router[0] = SensRadMax ; rinner[0] = router[0] - pipe_th; 
       zpl[1] = -230.*mm ; router[1] = 24.*mm ; rinner[1] = router[1] - pipe_th; 
       zpl[2] =  -80.*mm ; router[2] = 15.*mm ; rinner[2] = router[2] - pipe_th; 
       zpl[3] =  - zpl[2]; router[3] = router[2] ; rinner[3] = rinner[2]; 
       zpl[4] =  - zpl[1]; router[4] = router[1] ; rinner[4] = rinner[1]; 
       zpl[5] =  - zpl[0]; router[5] = router[0] ; rinner[5] = rinner[0]; 
       // central conical beryllium pipe 
     G4Polycone *solidCentralPipe = new G4Polycone( "CentralBeamPipe",
                            startPhi, endPhi,
                            numz, zpl, rinner, router);
     G4LogicalVolume *logicCentralTube = new G4LogicalVolume( solidCentralPipe,
                                BeamPipeMat,
                                "logicCentralTube",
                                0, 0, 0);
     // central vacuum filler
         for ( int i=0; i < numz ; i++) {
       router[i] = rinner[i];
       rinner[i] = 0.;
     }

     G4Polycone *solidCentralVac = new G4Polycone( "CentralBeamPipeVac",
                            startPhi, endPhi,
                            numz, zpl, rinner, router);
     G4LogicalVolume *logicCentralTubeVac = new G4LogicalVolume( solidCentralVac,
                                Vacuum,
                                "logicCentralTubeVac",
                                0, 0, 0);
     // place vacuum filler into world

     new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
               logicCentralTubeVac, "CentralPipeVac",
               logicWorld, false, 0); 

 
     // place central part in World

     new G4PVPlacement(0, G4ThreeVector(0.,0.,0.),
               logicCentralTube, "CentralPipe",
               logicWorld, false, 1);
     // visual attributes 
     //
     logicCentralTube->SetVisAttributes( BeamPipeVisAtt );
     logicCentralTubeVac->SetVisAttributes( VacuumVisAtt );
     G4cout << "                             ... CentralBeamPipe            done! " << G4endl; 
      
     //
         // central pipe endcaps
     //-------------------------------------------------------------------------------
     G4double dy = innerRad - LcalToBeamTol;
     G4double dx = dy / cos( rotAng );
     G4double hdz = pipe_th /2.;
         G4double dxshift = zpl[numz-1]*tan( rotAng );

     // temporary solid endcap
     G4Tubs *solidtmp = new G4Tubs ( "solidtmp", 0., SensRadMax, hdz, startPhi, endPhi);
         // the puncher ( to make a hole for EndBeamPipe in endcap )
     G4EllipticalTube *ksolidPunch = new G4EllipticalTube( "solidPunch", dx, dy, pipe_th);

     G4SubtractionSolid *solidEndcap = new G4SubtractionSolid( "solidEndcap",
                                   solidtmp, 
                                   ksolidPunch,
                                   0,
                                   G4ThreeVector( dxshift, 0., 0.));

        G4LogicalVolume *logicLCalFrntTubeWall = new G4LogicalVolume( solidEndcap, BeamPipeMat,
                               "logicLCalFrntTubeWall", 0, 0, 0);

     // place endcaps in world
     // left endcap 
     new G4PVPlacement(0, G4ThreeVector( 0., 0., zpl[0]-hdz),
               logicLCalFrntTubeWall,"LCalFront1",
               logicWorld, false, 1);
     // right endcap
     new G4PVPlacement(0, G4ThreeVector( 0., 0., zpl[numz-1]+hdz),
               logicLCalFrntTubeWall,"LCalFront2",
               logicWorld, false, 2);
     logicLCalFrntTubeWall->SetVisAttributes( BeamPipeVisAtt);

     G4cout << "                             ... LCal front wall            done! " << G4endl; 
   

     // LCAL inner pipe 
     // this pipes are to be rotated and shifted
     // to avoid overlaps , their inner ( IP side ) ends
     // must be clipped  
     //---------------------------------------------------------------------------------- 
     G4double rmax = innerRad - LcalToBeamTol;
         G4double rmin = rmax - pipe_th;
     G4double hendDZ = (Lcal_inner_pipe_zend -zpl[numz-1])/2.;
     G4double z0 = zpl[numz-1] + hendDZ;
     G4Transform3D transinner1( G4RotationMatrix().rotateZ(180.*deg).rotateY(rotAng1),
                    G4ThreeVector( 0., 0., z0).rotateZ(180.*deg).rotateY(rotAng1));
     G4Transform3D transinner2( G4RotationMatrix().rotateY(rotAng2),
                  G4ThreeVector( 0., 0., z0).rotateY(rotAng2));

     // solid shape depends on beam crossing angle
     //
     if ( rotAng == 0. ){  // here simple tube

       G4Tubs *solidEndPipe = new G4Tubs ( "solidEndPipe", rmin, rmax, hendDZ, startPhi, endPhi);  // solid pipe
           G4Tubs *solidEndVac  = new G4Tubs ( "solidEndVac" , 0.  , rmin, hendDZ, startPhi, endPhi);  // solid vacuum filler


       // logical volumes 

         logicLCalInnerTube = new G4LogicalVolume( solidEndPipe, BeamPipeMat, "logicLCalInnerTube", 0, 0, 0); 
         logEndVac  = new G4LogicalVolume( solidEndVac , Vacuum     , "logicLcalInnerTubeVac" , 0, 0, 0);
     }
     else  // tube must be clipped at IP side 
       {
         hendDZ += rmax*tan( rotAng2 );  // in this case tube must be a bit longer
       G4Tubs *solidend = new G4Tubs ( "solidend", rmin, rmax, hendDZ, startPhi, endPhi);  // tmp pipe
           G4Tubs *solidvac = new G4Tubs ( "solidvac",   0., rmin, hendDZ, startPhi, endPhi);  // tmp vacuum filler
       G4double clipDZ = 2.*rmax*sin( rotAng2 );
       G4Tubs *solidclip = new G4Tubs ( "solidclip",   0., 2.*rmax, clipDZ, startPhi, endPhi);  // tmp clipper a "bit" bigger

           // clipper must be shifted and rotated
           G4ThreeVector zshift( 0., 0., -hendDZ ); //-clipDZ);
           G4RotationMatrix *yRot = new G4RotationMatrix();
           yRot->rotateY( rotAng2 );

         // final clipped solids

       G4SubtractionSolid *solidEndPipe = new G4SubtractionSolid( "solidEndPipe", solidend, solidclip, yRot, zshift); // pipe at z-
       G4SubtractionSolid *solidEndVac  = new G4SubtractionSolid( "solidEndVac", solidvac, solidclip, yRot, zshift); // vac at z-

       // logical volumes 

         logicLCalInnerTube = new G4LogicalVolume( solidEndPipe, BeamPipeMat, "logicLCalInnerTube", 0, 0, 0); 
         logEndVac  = new G4LogicalVolume( solidEndVac , Vacuum     , "logicLcalInnerTubeVac" , 0, 0, 0);
       }


         // place vacuum pipes into world separately
         // left end pipe
         new G4PVPlacement( transinner1,
                logEndVac,
                "LcalInnerTubeVac1",
                logicWorld,
                false,
                1);


        // right end pipe
         new G4PVPlacement( transinner2,
                logEndVac,
                "LcalInnerTubeVac2",
                logicWorld,
                false,
                2);
         // place pipes into world
         // left end pipe
         new G4PVPlacement( transinner1,
                logicLCalInnerTube,
                "LcalInnerTube1",
                logicWorld,
                false,
                1);


        // right end pipe
         new G4PVPlacement( transinner2,
                logicLCalInnerTube,
                "LcalInnerTube2",
                logicWorld,
                false,
                2);
         // set visual attributes

     logicLCalInnerTube->SetVisAttributes ( BeamPipeVisAtt);
     logEndVac->SetVisAttributes( VacuumVisAtt );

     G4cout << "                             ... LCal inner Pipe            done! " << G4endl; 
      //
       //  LCAL inner -> BCAL inner pipe transition wall
       //
     G4double LC_BC_wallZ = Lcal_inner_pipe_zend +rmax*tan( rotAng2 ) +pipe_th/2.;
     G4double LC_BC_wall_rmax = innerRad - LcalToBeamTol;
     G4double LC_BC_wall_rmin = BCal_inner_outube_rmax;
     //
     G4LogicalVolume *logicBCalFrntTubeWall = NULL;
     //
     if ( rotAng == 0 )      // zero crossing angle - only one hole for out/in beam lines
       {
         G4Tubs *solidLC_BC  = new G4Tubs ( "solidLC_BC", LC_BC_wall_rmin, LC_BC_wall_rmax, pipe_th/2., startPhi, endPhi); 
         logicBCalFrntTubeWall = new G4LogicalVolume ( solidLC_BC, Iron, "logicBCalFrntTubeWall", 0, 0, 0);
       }
     else                    // non zero crossing angle, need punch hole for incomoming beam line 
       {
         G4double xd = -2.*LC_BC_wallZ*tan( rotAng );
         G4ThreeVector xshift ( xd, 0., 0.);
         G4RotationMatrix *yRotHole = new G4RotationMatrix();
         yRotHole->rotateY(  -rotAng );

         G4Tubs *solidtmp   = new G4Tubs ( "solidLC_BC", LC_BC_wall_rmin, LC_BC_wall_rmax, pipe_th/2., startPhi, endPhi); 
         G4Tubs *solidhole  = new G4Tubs ( "solidhole" ,              0., BCal_inner_intube_rmax, 4.*pipe_th, startPhi, endPhi);
         G4SubtractionSolid *solidLC_BC = new G4SubtractionSolid("solidLC_BC", solidtmp, solidhole, yRotHole, xshift);
  
         logicBCalFrntTubeWall = new G4LogicalVolume ( solidLC_BC, Iron, "logicBCalFrntTubeWall", 0, 0, 0);
       }

     G4Transform3D transwall1( G4RotationMatrix().rotateZ(180.*deg).rotateY(rotAng1),
                   G4ThreeVector( 0., 0., LC_BC_wallZ).rotateZ(180. *deg).rotateY(rotAng1));
     G4Transform3D transwall2( G4RotationMatrix().rotateY(rotAng2),
                  G4ThreeVector( 0., 0., LC_BC_wallZ).rotateY(rotAng2));
     // place wall, and out/in pipe line
     //
     new G4PVPlacement ( transwall1, logicBCalFrntTubeWall, "BCalFrontWall-1", logicWorld, false, 1);
     new G4PVPlacement ( transwall2, logicBCalFrntTubeWall, "BCalFrontWall-2", logicWorld, false, 2);

     logicBCalFrntTubeWall->SetVisAttributes ( BeamPipeVisAtt);

     G4cout << "                              ...BCal front Wall            done! " << G4endl; 

     if ( Setup::Build_BCal == "Yes" ) {
     //
     // incoming and outgoing beam lines tubes
     //
     // outgoing beam line dimensions
     //
         G4double OutLine_rmax = BCal_inner_outube_rmax; 
         G4double OutLine_rmin = BCal_inner_outube_rmax - pipe_th;
     G4double OutLine_zstart = Lcal_inner_pipe_zend+rmax*tan( rotAng2 ); 
     G4double OutLine_zend   = Setup::world_hdz - 1*mm; 
     G4double OutLine_hDZ    = (OutLine_zend - OutLine_zstart)/2. ; 
     G4double OutLine_center    = OutLine_zstart + OutLine_hDZ; 
     //
     // incoming beam line dimensions
     //
     G4double InLine_rmax = BCal_inner_intube_rmax;
     G4double InLine_rmin = BCal_inner_intube_rmax - pipe_th;
     G4double InLine_hDZ  = OutLine_hDZ ; 
     G4double InLine_center    = OutLine_center ; 

     G4Tubs *solidOutLine = new G4Tubs( "solidOutLine", OutLine_rmin, OutLine_rmax, OutLine_hDZ, startPhi, endPhi ); 
     G4Tubs *solidOutVac = new G4Tubs( "solidOutVac"  ,           0., OutLine_rmin, OutLine_hDZ, startPhi, endPhi ); 

     G4LogicalVolume *logicBeamOutTube = new G4LogicalVolume( solidOutLine, Iron, "logicBeamOutTube", 0, 0, 0);
     G4LogicalVolume *logicOutVac  = new G4LogicalVolume( solidOutVac, Vacuum, "logicOutVac", 0, 0, 0);

     G4Transform3D transOut1( G4RotationMatrix().rotateY(rotAng1),
                  G4ThreeVector( 0., 0., OutLine_center).rotateY(rotAng1));
     G4Transform3D transOut2( G4RotationMatrix().rotateY(rotAng2),
                  G4ThreeVector( 0., 0., OutLine_center).rotateY(rotAng2));

     new G4PVPlacement ( transOut1, logicBeamOutTube, "OutBeamTube1", logicWorld, false, 1); 
     new G4PVPlacement ( transOut1, logicOutVac , "OutBeamVac1", logicWorld, false, 1); 
     new G4PVPlacement ( transOut2, logicBeamOutTube, "OutBeamTube2", logicWorld, false, 1); 
     new G4PVPlacement ( transOut2, logicOutVac , "OutBeamVac2", logicWorld, false, 1); 

     logicBeamOutTube->SetVisAttributes ( BeamPipeVisAtt );
     logicOutVac->SetVisAttributes ( VacuumVisAtt );
     G4cout << "                             ... BCAL inner Downstream tube done! " << G4endl;
     // 
     if ( rotAng != 0 )                // need separate tube for incoming beam
       {
         G4Transform3D transIn1( G4RotationMatrix().rotateZ(180.*deg).rotateY(-rotAng1),
                     G4ThreeVector( 0., 0., InLine_center).rotateZ(180.*deg).rotateY(-rotAng1));
         G4Transform3D transIn2( G4RotationMatrix().rotateY(-rotAng2),
                     G4ThreeVector( 0., 0., InLine_center).rotateY(-rotAng2));
     // solids must be clipped at IP side
     G4double clip_rmax = 2.*InLine_rmax / cos ( rotAng );
     G4double clip_hdz  = 2.*InLine_rmax * tan( 2.*rotAng );
         G4ThreeVector zclip ( 0., 0., -InLine_hDZ );
     G4RotationMatrix *yrotC = new G4RotationMatrix();
     yrotC->rotateY(  -2.*rotAng );

         G4Tubs *solidLineTmp = new G4Tubs( "solidInLine", InLine_rmin, InLine_rmax, InLine_hDZ, startPhi, endPhi ); 
         G4Tubs *solidVacTmp  = new G4Tubs( "solidInVac" ,          0., InLine_rmin, InLine_hDZ, startPhi, endPhi );
             G4Tubs *clipper      = new G4Tubs( "clipper"    ,          0., clip_rmax  , clip_hdz  , startPhi, endPhi );

         // clipped solids
         G4SubtractionSolid *solidInLine = new G4SubtractionSolid( "solidInLine", solidLineTmp, clipper, yrotC, zclip);
         G4SubtractionSolid *solidInVac  = new G4SubtractionSolid( "solidInVac" , solidVacTmp , clipper, yrotC, zclip);
          

         G4LogicalVolume *logicBeamInTube = new G4LogicalVolume( solidInLine, Iron, "logicBeamInTube", 0, 0, 0);
         G4LogicalVolume *logicInVac  = new G4LogicalVolume( solidInVac, Vacuum, "logicInVac", 0, 0, 0);

         new G4PVPlacement ( transIn1, logicBeamInTube, "BeamInTube1", logicWorld, false, 1); 
         new G4PVPlacement ( transIn1, logicInVac , "InBeamVac1", logicWorld, false, 1);
         new G4PVPlacement ( transIn2, logicBeamInTube, "BeamInTube2", logicWorld, false, 1);
         new G4PVPlacement ( transIn2, logicInVac , "InBeamVac2", logicWorld, false, 1);

         logicBeamInTube->SetVisAttributes ( BeamPipeVisAtt );
         logicInVac->SetVisAttributes ( VacuumVisAtt );
       }
 
     G4cout << "                             ... BCAL inner Upstream tube   done! " << G4endl;

     }



     G4cout << "                             . ..BeamPipe                  done !" << G4endl;
     G4cout << G4endl;
}



void LCDetectorConstruction::BuildLHcal()
{
  G4cout <<  " Building LHCAL ..." << G4endl; 
    G4double zpos = LHcal_zbegin + LHcal_hDZ;

    G4Transform3D trans1( G4RotationMatrix().rotateY(rotAng1),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng1));
    G4Transform3D trans2( G4RotationMatrix().rotateY(rotAng2),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng2));

    G4Box *solidtmp = new G4Box ( "solidtmp", LHcal_rmax, LHcal_rmax, LHcal_hDZ );
    G4Tubs *solidpunch = new G4Tubs ( "solidpunch", 0., LHcal_rmin, 2.*LHcal_hDZ, startPhi, endPhi );
    G4SubtractionSolid *solidLHcal = new G4SubtractionSolid( "solidLHcal", solidtmp, solidpunch, 0, G4ThreeVector());
    G4LogicalVolume *logicLHcal = new G4LogicalVolume( solidLHcal, LHcalMat, "logicLHcal", 0, 0, 0);
 
                  new G4PVPlacement( trans1 ,
                                     logicLHcal,
                                     "LHcal1",
                                     logicWorld,
                                     false,
                                     1);
                  new G4PVPlacement( trans2 ,
                                     logicLHcal,
                                     "LHcal2",
                                     logicWorld,
                                     false,
                                     2);
     G4VisAttributes *LHcalVisAtt = new G4VisAttributes(G4Colour(0.7, 0.3, 0.7));
  
       if(Setup::LHcal_VisSolid ) LHcalVisAtt->SetForceSolid(true);
        else LHcalVisAtt->SetForceWireframe(true);
     logicLHcal->SetVisAttributes(LHcalVisAtt);

    // define a subdetector region for LHCAL 
    regionLHcal = new G4Region("LHCAL");
    logicLHcal->SetRegion(regionLHcal);
    regionLHcal->AddRootLogicalVolume( logicLHcal );

  
  G4cout <<  "                   .....done ! " << G4endl; 
}

void LCDetectorConstruction::BuildBCal()
{

  G4cout <<  " Building BCAL ..." ; 
  // whole BCAL set ( PM, Shielding, BCAL
  // helper volume
    G4double BCal_set_zlength = BCal_dPairMoni + BCal_dgraphite + BCal_zlength;
    G4double BCal_set_hDZ = BCal_set_zlength/2.;
    G4double BCal_set_zpos = BCal_zbegin + BCal_set_hDZ ;
    // z-positions in local HLP system
    G4double zposPM = -BCal_set_hDZ + BCal_dPairMoni/2.;
    G4double zposGS = zposPM + BCal_dPairMoni/2. + BCal_dgraphite/2.;
    G4double zposBC = zposGS + BCal_dgraphite/2. +BCal_zlength/2.;
    // global transformation
    G4Transform3D trans1( G4RotationMatrix().rotateZ(180. *deg).rotateY(rotAng1),
                          G4ThreeVector( 0., 0., BCal_set_zpos).rotateZ( 180.*deg).rotateY(rotAng1));
    G4Transform3D trans2( G4RotationMatrix().rotateY(rotAng2),
                          G4ThreeVector( 0., 0., BCal_set_zpos).rotateY(rotAng2));
    // logical volumes
    G4LogicalVolume *logicBCal = NULL;
    G4LogicalVolume *logicPM = NULL;
    G4LogicalVolume *logicGS = NULL;
    G4LogicalVolume *logicBC = NULL;

    if ( rotAng == 0 ) {

  // Whole BCAL set - helper volume
    G4Tubs *solidHLP = new G4Tubs ( "solidHLP", BCal_rmin, BCal_rmax, BCal_set_hDZ, startPhi, endPhi);
    logicBCal = new G4LogicalVolume( solidHLP, Air, "logicBCal", 0, 0, 0);

  // Pair Monitor
 
     G4Tubs *solidPM = new G4Tubs ( "solidPM", BCal_rmin, BCal_rmax, BCal_dPairMoni/2., startPhi, endPhi);
     logicPM = new G4LogicalVolume( solidPM, Silicon, "logicPM", 0, 0, 0);
     new G4PVPlacement( 0, G4ThreeVector( 0., 0., zposPM), logicPM, "PairMonitor",logicBCal, false, 1);

  // Graphite Shielding
 
     G4Tubs *solidGS = new G4Tubs ( "solidGr", BCal_rmin, BCal_rmax, BCal_dgraphite/2., startPhi, endPhi);
     logicGS = new G4LogicalVolume( solidGS, Graphite, "logicGS", 0, 0, 0);
     new G4PVPlacement( 0, G4ThreeVector( 0., 0., zposGS), logicGS, "GraphiteShield", logicBCal, false, 1);

  // BCAL

    G4Tubs *solidBC = new G4Tubs ( "solidBC", BCal_rmin, BCal_rmax, BCal_zlength/2., startPhi, endPhi);
    logicBC = new G4LogicalVolume( solidBC, BCalMat, "logicBC", 0, 0, 0);
    new G4PVPlacement(  0, G4ThreeVector( 0., 0., zposBC), logicBC, "GraphiteShield", logicBCal,  false, 1);

    } else {

      // need to punch a hole for incoming beam tube, in each component of BCAL set
      //  craete solid puncher

      G4double puncher_rmax = BCal_inner_intube_rmax + 2. *mm;
      G4double puncher_dz = BCal_set_zlength;
      G4RotationMatrix *yrotB = new G4RotationMatrix();
      yrotB->rotateY(  2.*rotAng );

      G4Tubs *puncher     = new G4Tubs( "puncher", 0., puncher_rmax, puncher_dz, startPhi, endPhi );
      // temporary volumes for punching
      G4Tubs *solidHLPtmp = new G4Tubs ( "solidtmp1", BCal_rmin, BCal_rmax, BCal_set_hDZ, startPhi, endPhi);
      G4Tubs *solidPMtmp  = new G4Tubs ( "solidtmp2", BCal_rmin, BCal_rmax, BCal_dPairMoni/2., startPhi, endPhi);
      G4Tubs *solidGStmp  = new G4Tubs ( "solidtmp3", BCal_rmin, BCal_rmax, BCal_dgraphite/2., startPhi, endPhi);
      G4Tubs *solidBCtmp  = new G4Tubs ( "solidtmp4", BCal_rmin, BCal_rmax, BCal_zlength/2., startPhi, endPhi);
      // punched solids
      G4double xsh = BCal_set_zpos * tan( -2.*rotAng );
      G4SubtractionSolid *solidHLP = new G4SubtractionSolid("solidHLP", solidHLPtmp, puncher, yrotB, G4ThreeVector( xsh, 0., 0.)); 
      xsh = (BCal_zbegin+BCal_dPairMoni/2.) * tan( -2.*rotAng );
      G4SubtractionSolid *solidPM = new G4SubtractionSolid("solidPM", solidPMtmp, puncher, yrotB, G4ThreeVector( xsh, 0., 0.)); 
      xsh = (BCal_zbegin+BCal_dPairMoni+BCal_dgraphite/2.) * tan( -2.*rotAng );
      G4SubtractionSolid *solidGS = new G4SubtractionSolid("solidGS", solidGStmp, puncher, yrotB, G4ThreeVector( xsh, 0., 0.)); 
      xsh = (BCal_zbegin+BCal_dPairMoni+BCal_dgraphite + BCal_zlength/2.) * tan( -2.*rotAng );
      G4SubtractionSolid *solidBC = new G4SubtractionSolid("solidBC", solidBCtmp, puncher, yrotB, G4ThreeVector( xsh, 0., 0.)); 
      // logical volumes
      logicBCal = new G4LogicalVolume( solidHLP, Air, "logicBCal", 0, 0, 0);

      logicPM = new G4LogicalVolume( solidPM, Silicon, "logicPM", 0, 0, 0);
      new G4PVPlacement( 0, G4ThreeVector( 0., 0., zposPM), logicPM, "PairMonitor",logicBCal, false, 1);

      logicGS = new G4LogicalVolume( solidGS, Graphite, "logicGS", 0, 0, 0);
      new G4PVPlacement( 0, G4ThreeVector( 0., 0., zposGS), logicGS, "GraphiteShield", logicBCal, false, 1);

      logicBC = new G4LogicalVolume( solidBC, BCalMat, "logicBC", 0, 0, 0);
      new G4PVPlacement(  0, G4ThreeVector( 0., 0., zposBC), logicBC, "BCAL", logicBCal,  false, 1);
   } 


  // whole BCAL set into world
  new G4PVPlacement ( trans1, logicBCal, " BCal1 ", logicWorld, false, 1); 
  new G4PVPlacement ( trans2, logicBCal, " BCal2 ", logicWorld, false, 2); 

  //
  // Visual Attributes                                                           
  //
     G4VisAttributes *PMVisAtt = new G4VisAttributes(G4Colour(1., 0.0, 0.0));
     if(Setup::BCal_VisSolid ) PMVisAtt->SetForceSolid(true);
                             else PMVisAtt->SetForceWireframe(true);
     logicPM->SetVisAttributes(PMVisAtt);

     G4VisAttributes *GraphiteVisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
     if(Setup::BCal_VisSolid ) GraphiteVisAtt->SetForceSolid(true);
                          else GraphiteVisAtt->SetForceWireframe(true);
     logicGS->SetVisAttributes(GraphiteVisAtt);

     G4VisAttributes *BCalVisAtt = new G4VisAttributes(G4Colour(0.7, 0.3, 0.7));
     if(Setup::BCal_VisSolid ) BCalVisAtt->SetForceSolid(true);
                         else  BCalVisAtt->SetForceWireframe(true);
     logicBC->SetVisAttributes(BCalVisAtt);

    // define a subdetector region for BCAL 
    regionBCal = new G4Region("BCAL");
    logicBCal->SetRegion(regionBCal);
    regionBCal->AddRootLogicalVolume( logicBCal );


  G4cout <<  "                   .....done ! " << G4endl; 
}

void LCDetectorConstruction::BuildMask()
{
  G4cout <<  " Building Mask...              " ; 
  G4double Mask_hDZ = (Setup::world_hdz - Mask_zstart )/2.;
  G4double zFrnt = Mask_zstart+Mask_thickness/2.;  
  G4double zCent = Mask_zstart+Mask_hDZ;  
  G4double x0    = zFrnt * tan( rotAng );
  G4ThreeVector xzdist( x0 , 0. , -Mask_hDZ+Mask_thickness/2.);

  G4Box *solidtmp0  = new G4Box( "solidtmp0", Mask_hDX, Mask_hDY, Mask_hDZ );
  G4Box *solidRoom  = new G4Box( "solidRoom",Mask_hDX-Mask_thickness, Mask_hDY-Mask_thickness, Mask_hDZ);

  G4SubtractionSolid *solidtmp1 = 
    new G4SubtractionSolid( "solidtmp1", solidtmp0, solidRoom, 0, G4ThreeVector( 0., 0., Mask_thickness));
  G4Tubs *solidhole = 
    new G4Tubs( "solidhole", 0., Mask_rmin_at_zstart, Mask_thickness, startPhi, endPhi); 
  G4SubtractionSolid *solidMask = 
    new G4SubtractionSolid( "solidMask", solidtmp1, solidhole, 0, xzdist ); 
  
          G4LogicalVolume *logicMask = new G4LogicalVolume( solidMask    , Mask_Mat, "logicMask", 0, 0, 0); 
          G4RotationMatrix *yrotMSK = new G4RotationMatrix();
          yrotMSK->rotateY( 180. *deg).rotateZ(180 *deg); 
          new G4PVPlacement ( yrotMSK , G4ThreeVector(0., 0.,-zCent), logicMask, "Mask1", logicWorld, false, 1);  
          new G4PVPlacement ( 0, G4ThreeVector(0., 0., zCent), logicMask, "Mask2", logicWorld, false, 2); 

     G4VisAttributes *MaskVisAtt = new G4VisAttributes(G4Colour(0.7, 0.3, 0.3));
     if(Setup::Mask_VisSolid ) MaskVisAtt->SetForceSolid(true);
                         else  MaskVisAtt->SetForceWireframe(true);
 
     logicMask->SetVisAttributes(MaskVisAtt);

    // define a subdetector region for MASK 
    regionMask = new G4Region("MASK");
    logicMask->SetRegion(regionMask);
    regionMask->AddRootLogicalVolume( logicMask );


  G4cout <<  "                   .....done ! " << G4endl; 
}


// =========== CELL PARAMETERIZATION ============
LCCellParam::LCCellParam(G4int    NoCells,
                         G4double startR,
                         G4double endR,
             G4double SensHalfZ,
                         G4double SihalfZ,
                         G4double AlhalfZ,
                         G4double clipSize,
                         G4double startPhi,
                         G4double deltaPhi)
{
    lNoCells  = NoCells;
    lstartR   = startR + clipSize;
    lendR     = endR - clipSize;
    lSenshalfZ= SensHalfZ;
    lSihalfZ  = SihalfZ;
    lAlhalfZ  = AlhalfZ;
    lstartPhi = startPhi;
    ldeltaPhi = deltaPhi;
    lclipSize = ( clipSize > 0. ) ?  clipSize + 0.05: 0.;
    ldeltaR   = (lendR - lstartR)/(G4double)NoCells;
}

LCCellParam::~LCCellParam() {}

void LCCellParam::ComputeTransformation(const G4int, G4VPhysicalVolume *physVol ) const
{
  physVol->SetTranslation(  G4ThreeVector(0., 0., 0));
  physVol->SetRotation(0);
}

void LCCellParam::ComputeDimensions(G4Tubs &Cell, const G4int copyNo,
                                    const G4VPhysicalVolume* physVol ) const
{
    G4double innerRad = lstartR + copyNo * ldeltaR;
    G4double midRad   = innerRad + ldeltaR/2;
    G4double outerRad = innerRad + ldeltaR;
    G4double cutPhi   = atan(lclipSize / midRad) *rad ;
    G4double delPhi   = ldeltaPhi - cutPhi;
    G4double startPhi = lstartPhi;
    G4double metPhi   = atan(0.05/midRad)*rad;
    G4double halfZ    = lSihalfZ;

    G4String MotherLogName = physVol->GetMotherLogical()->GetName();

    if ( MotherLogName.contains("MetSector") ){
       innerRad +=  0.05;
       outerRad -=  0.05;
           delPhi   -= metPhi;
           startPhi += metPhi;
           halfZ     = lAlhalfZ; 
     }

    Cell.SetInnerRadius(innerRad);
    Cell.SetOuterRadius(outerRad);
    Cell.SetZHalfLength(halfZ); 

    if ( MotherLogName.contains("Sector1") )
      {      Cell.SetStartPhiAngle( lstartPhi + cutPhi);
         Cell.SetDeltaPhiAngle( delPhi    ); }
    else if ( MotherLogName.contains("Sector4") )
      {      Cell.SetStartPhiAngle( lstartPhi );
         Cell.SetDeltaPhiAngle( delPhi    ); }
    else 
      {      Cell.SetStartPhiAngle( lstartPhi );
         Cell.SetDeltaPhiAngle( ldeltaPhi ); }

}



void LCDetectorConstruction::Print()
  {
    G4double Sens_zbeg = Setup::Lcal_sens_Z0;
    G4double Sens_zend = Setup::Lcal_sens_Z0 + G4double(nLayers)*SensDZ ;    
    G4double theta_min = (SensRadMin+deadPhi+CellPitch/2.)/Sens_zend;
    G4double theta_max = (SensRadMax-deadPhi-CellPitch/2.)/Sens_zbeg;
    G4double theta_fid_min = (SensRadMin+deadPhi+CellPitch/2.)/Sens_zbeg;
    G4double theta_fid_max = (SensRadMax-deadPhi-CellPitch/2.)/Sens_zend;
    G4double FE_space_thetamin = FECave_rmin / Sens_zend;
    G4double FE_space_thetamax = FECave_rmax / Sens_zbeg;
    G4double  dphi = 360.*sectorPhi / CLHEP::twopi;
    G4double  tile_gap = 2.*(Setup::Lcal_sector_dead_gap + 0.05);

    G4cout << G4endl;
    printf ("%s\n"," ======================================================");
    printf ("%s\n"," |         LCAL parameters used in this run           |");
    printf ("%s\n"," ======================================================");
    printf ("%s\n"," |                                                    |");
    printf ("%s %4d %s \n"," |      Number of radial cells : ",nCells,"               |");      
    printf ("%s %4d %s \n"," |      Number of sectors      : ",nSectors,"               |"); 
    printf ("%s %4d %s \n"," |      Number of tiles        : ",nTiles,"               |"); 
    printf ("%s %4d %s \n"," |      Number of planes       : ",nLayers,"               |"); 
    printf ("%s\n"," |                                                    |");
    printf ("%s %6.1f %s \n"," |      LCAL length            : ",2.*hLumiCalDZ,"   [mm]      |"); 
    printf ("%s %6.2f %s \n"," |      LCAL z begin           : ",Lcal_zbegin,"  [mm]      |"); 
    printf ("%s %6.2f %s \n"," |          first sensor z-pos : ",Sens_zbeg,"  [mm]      |"); 
    printf ("%s %6.1f %s \n"," |      LCAL inner radius      : ",innerRad,"   [mm]      |"); 
    printf ("%s %6.1f %s \n"," |         sensor inner radius : ",SensRadMin,"   [mm]      |"); 
    printf ("%s %6.1f %s \n"," |      LCAL outer radius      : ",outerRad,"   [mm]      |"); 
    printf ("%s %6.1f %s \n"," |         sensor outer radius : ",SensRadMax,"   [mm]      |"); 
    printf ("%s\n"," |                                                    |");
    printf ("%s %8.3f %s \n"," |           cell radial pitch : ",CellPitch," [mm]      |"); 
    printf ("%s %6.1f %s \n"," |                sector width : ",dphi,"   [deg]     |"); 
    printf ("%s %8.3f %s \n"," |           sensor z distance : ",SensDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," | gap between absorber plates : ",Setup::Lcal_absorber_gap," [mm]      |");
    printf ("%s %8.3f %s \n"," |                     air gap : ",Setup::Lcal_layer_gap," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |                    tile gap : ",tile_gap," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |           layers phi offset : ",Setup::Lcal_layers_phi_offset / deg," [deg]     |"); 
    printf ("%s\n"," |                                                    |");
    printf ("%s %8.3f %s \n"," |          F-E chip thickness : ",2.*FEChip_hDZ," [mm]      |");
    if ( Setup::Lcal_FEChip_rmin < Setup::Lcal_SensRadMax ) {
    printf ("%s %8.3f %s \n"," |   PCB interconnection plate : ",Setup::Lcal_PCB_thickness," [mm]      |");
    }else{
    printf ("%s %8.3f %s \n"," |      FANOUT front thickness : ",2.*hFanoutFrontDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |             back  thickness : ",2.*hFanoutBackDZ," [mm]      |"); 
    }
    printf ("%s %8.3f %s \n"," |       Si   sensor thickness : ",2.*hSiliconDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |  Pad metalization thickness : ",2.*hMetalDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |          Tungsten thickness : ",2.*hTungstenDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |       total plane thickness : ",2.*hLayerDZ," [mm]      |"); 
    printf ("%s %8.3f %s \n"," |     mass of the LCAL (1 arm): ",logicWholeLC->GetMass()/kg," [kg]      |"); 
    printf ("%s\n"," |                                                    |");
    printf ("%s %4d %s\n",   " |                use absorber : ",Setup::Lcal_use_absorber,"               |");
    printf ("%s %4d %s\n",   " |                  use fanout : ",Setup::Lcal_use_fanout  ,"               |"); 
    printf ("%s\n"," |                                                    |");
    printf ("%s\n"," ======================================================");
    printf ("%s\n"," |      LCAL polar angle theta acceptance [rad]       |"); 
    printf ("%s\n"," ======================================================");
    printf ("%s\n"," |               |    geometrical          fiducial   |"); 
    printf ("%s\n"," ------------------------------------------------------");
    printf ("%s%10.3f%s%8.3f%s\n"," |    theta_min  |   ",theta_min,"           ",theta_fid_min,"    |"); 
    printf ("%s%10.3f%s%8.3f%s\n"," |    theta_max  |   ",theta_max,"           ",theta_fid_max,"    |"); 
    printf ("%s%10.3f%s\n"," | FE-theta_min  |   ",FE_space_thetamin,"                       |"); 
    printf ("%s%10.3f%s\n"," | FE-theta_max  |   ",FE_space_thetamax,"                       |"); 
    printf ("%s\n"," ======================================================");
    G4cout << G4endl;
  }

