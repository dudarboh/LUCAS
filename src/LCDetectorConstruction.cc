#include "LCDetectorConstruction.hh"
#include "LCSDTrigger.hh"
#include "LCSD.hh"

#include "G4TransportationManager.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVReplica.hh"

#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Element.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

#ifdef RUN_PH
G4ThreadLocal G4UniformMagField* LCDetectorConstruction::fMagField = 0;
G4ThreadLocal G4FieldManager* LCDetectorConstruction::fFieldMgr = 0;
#endif

G4VPhysicalVolume* LCDetectorConstruction::Construct(){
    // std::cout<<"Start of LCDetectorConstruction::Construct"<<std::endl;

    //***Build materials***//
    G4NistManager *materials = G4NistManager::Instance();

    // For epoxy glue
    G4Element *H = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element *C = new G4Element("Carbon", "C", 6., 12.01*g/mole);
    G4Element *O = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
    //For World
    G4Material *Air = materials->FindOrBuildMaterial("G4_AIR");
    //For absorbers
    G4Material *W = materials->FindOrBuildMaterial("G4_W");
    G4Material *Ni = materials->FindOrBuildMaterial("G4_Ni");
    G4Material *Cu = materials->FindOrBuildMaterial("G4_Cu");
    //For fanout, carbon fiber and Sensor
    G4Material *Carbon = materials->FindOrBuildMaterial("G4_C");
    G4Material *Kapton = materials->FindOrBuildMaterial("G4_KAPTON");
    G4Material *Si = materials->FindOrBuildMaterial("G4_Si");
    G4Material *Al = materials->FindOrBuildMaterial("G4_Al");
    // For scintilator triggers before calorimeter
    G4Material *matSc = materials->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    // In the test beam 2 types of absorbers were used. They are slightly differ in their components
    //Absorber PL type
    G4Material *matAbsorberPL = new G4Material("matAbsorberPL", 18.0*g/cm3, 3);
    matAbsorberPL->AddMaterial(W, 0.95);
    matAbsorberPL->AddMaterial(Ni, 0.025);
    matAbsorberPL->AddMaterial(Cu, 0.025);

    //Absorber MSG type
    G4Material *matAbsorberMSG = new G4Material("matAbsorberMSG", 17.7*g/cm3, 3);
    matAbsorberMSG->AddMaterial(W,0.93);
    matAbsorberMSG->AddMaterial(Ni,0.0525);
    matAbsorberMSG->AddMaterial(Cu,0.0175);

    // Epoxy glue. It glues fanouts to Si sensor
    G4Material *Epoxy = new G4Material("Epoxy", 1.3*g/cm3, 3);
    Epoxy->AddElement(H, 0.1310);
    Epoxy->AddElement(C, 0.5357);
    Epoxy->AddElement(O, 0.3333);

    // Carbon fiber support for sensor
    G4Material *matCarbonFiber = new G4Material("matCarbonFiber", 1.6*g/cm3, 2);
    matCarbonFiber->AddMaterial(Carbon, 0.5);
    matCarbonFiber->AddMaterial(Epoxy, 0.5);

    //Front fanout material
    G4double massFrontFanoutEpoxy = Epoxy->GetDensity() * 0.065*mm;
    G4double massFrontFanoutKapton = Kapton->GetDensity() * 0.05*mm;
    G4double massFrontFanoutCu = Cu->GetDensity() * 0.035*mm;
    G4double massFrontFanout = (massFrontFanoutEpoxy + massFrontFanoutKapton + massFrontFanoutCu);
    G4double zFanout = 0.15*mm;
    G4double densFrontFanout = (massFrontFanoutEpoxy + massFrontFanoutKapton + massFrontFanoutCu)/zFanout;

    G4Material *matFrontFanout = new G4Material("matFrontFanout", densFrontFanout, 3);
    matFrontFanout->AddMaterial(Epoxy, massFrontFanoutEpoxy/massFrontFanout);
    matFrontFanout->AddMaterial(Kapton, massFrontFanoutKapton/massFrontFanout);
    matFrontFanout->AddMaterial(Cu, massFrontFanoutCu/massFrontFanout);

    //Back fanout material
    G4double massBackFanoutEpoxy = Epoxy->GetDensity() * 0.06*mm;
    G4double massBackFanoutKapton = Kapton->GetDensity() * 0.065*mm;
    G4double massBackFanoutCu = Cu->GetDensity() * 0.025*mm;
    G4double massBackFanout = (massBackFanoutEpoxy + massBackFanoutKapton + massBackFanoutCu);
    G4double densBackFanout = (massBackFanoutEpoxy + massBackFanoutKapton + massBackFanoutCu)/zFanout;

    G4Material *matBackFanout = new G4Material("matBackFanout", densBackFanout, 3);
    matBackFanout->AddMaterial(Epoxy, massBackFanoutEpoxy/massBackFanout);
    matBackFanout->AddMaterial(Kapton, massBackFanoutKapton/massBackFanout);
    matBackFanout->AddMaterial(Cu, massBackFanoutCu/massBackFanout);

    //***Build solids***//
    G4Box *solidWorld = new G4Box("World", 1200.*mm, 1200.*mm, 10000.*mm);

    // Trigger scintilators
    G4Box *solidSc1 = new G4Box("solidSc1", 15.*mm, 15.*mm, 2.064*mm);
    G4Box *solidSc2 = new G4Box("solidSc2", 15.*mm, 15.*mm, 2.064*mm);
    G4Box *solidSc3 = new G4Box("solidSc3", 15.*mm, 15.*mm, 14.*mm);

    //Telescope detectors from Test beam facility paper description
    G4Box *solidMimosaSi = new G4Box("solidMimosaSi", 10.6*mm, 5.3*mm, 0.025*mm);
    G4Box *solidMimosaKapton = new G4Box("solidMimosaKapton", 10.6*mm, 5.3*mm, 0.0125*mm);
    G4Box *solidMimosa26 = new G4Box("solidMimosa26", 10.6*mm, 5.3*mm, 0.05*mm);

#ifdef RUN_PH
    G4Box *solidTarget = new G4Box("solidTarget", 70.*mm, 70.*mm, 0.75*mm);
    G4Box *solidMagnet = new G4Box("solidMagnet", 750*mm, 175*mm, 500*mm);
#endif

    // Absorbers
    G4Box *solidAbsorberPL = new G4Box("solidAbsorberPL", 70.0*mm, 70.0*mm, 1.75*mm);
    G4Box *solidAbsorberMSG = new G4Box("solidAbsorberMSG", 70.0*mm, 70.0*mm, 1.785*mm);

    //Sensor consists of: FrontFanout-Al-Si-BackFanout. All inside CarbonFiber
    G4Box *solidCarbonFiber = new G4Box("solidCarbonFiber", 70.0*mm, 70.0*mm, 0.395*mm);
    G4Tubs *solidFanout = new G4Tubs("solidFanout", 80.*mm, 195.2*mm, 0.075*mm, 75.*deg, 30.*deg);
    G4Tubs *solidAl = new G4Tubs("solidAl", 80.*mm, 195.2*mm, 0.01*mm, 75.*deg, 30.*deg);

    //Mother volume where to put Si,Al, Fanout.
    // To access each pad separately in sensitive detector SensorStrip and SensorPad are replicated
    // inside Sensor
    G4Tubs *solidSensor = new G4Tubs("solidSensor", 80.*mm, 195.2*mm, 0.16*mm, 75.*deg, 30.*deg);
    G4Tubs *solidSensorStrip = new G4Tubs("solidSensorStrip", 80.*mm, 195.2*mm, 0.16*mm, -3.75*deg, 7.5*deg);
    G4Tubs *solidSensorPad = new G4Tubs("solidSensorPad", 80.*mm, 195.2*mm, 0.16*mm, -3.75*deg, 7.5*deg);


    //***Build logics***//
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, Air, "logicWorld", 0, 0, 0);
    fLogicSc1 = new G4LogicalVolume(solidSc1, matSc, "logicSc1", 0, 0, 0);
    fLogicSc2 = new G4LogicalVolume(solidSc2, matSc, "logicSc2", 0, 0, 0);
    fLogicSc3 = new G4LogicalVolume(solidSc3, matSc, "logicSc3", 0, 0, 0);

    // Mimosa telescope detector logic volumes
    G4LogicalVolume *logicMimosaSi = new G4LogicalVolume(solidMimosaSi, Si, "logicMimosaSi", 0, 0, 0);
    G4LogicalVolume *logicMimosaKapton = new G4LogicalVolume(solidMimosaKapton, Kapton, "logicMimosaKapton", 0, 0, 0);
    G4LogicalVolume *logicMimosa26 = new G4LogicalVolume(solidMimosa26, Air, "logicMimosa26", 0, 0, 0);

#ifdef RUN_PH
    G4LogicalVolume *logicTarget = new G4LogicalVolume(solidTarget, Cu, "logicTarget", 0, 0, 0); // Copper target
    fLogicMagnet = new G4LogicalVolume(solidMagnet, Air, "logicMagnet", 0, 0, 0);
#endif

    // Absorbers
    G4LogicalVolume *logicAbsorberPL = new G4LogicalVolume(solidAbsorberPL, matAbsorberPL, "logicAbsorberPL", 0, 0, 0);
    G4LogicalVolume *logicAbsorberMSG = new G4LogicalVolume(solidAbsorberMSG, matAbsorberMSG, "logicAbsorberMSG", 0, 0, 0);

    // Sensor parts
    G4LogicalVolume *logicCalCarbonFiber = new G4LogicalVolume(solidCarbonFiber, matCarbonFiber, "logicCalCarbonFiber", 0, 0, 0);
    G4LogicalVolume *logicTr1CarbonFiber = new G4LogicalVolume(solidCarbonFiber, matCarbonFiber, "logicTr1CarbonFiber", 0, 0, 0);
    G4LogicalVolume *logicTr2CarbonFiber = new G4LogicalVolume(solidCarbonFiber, matCarbonFiber, "logicTr2CarbonFiber", 0, 0, 0);
    G4LogicalVolume *logicFrontFanout = new G4LogicalVolume(solidFanout, matFrontFanout, "logicFrontFanout", 0, 0, 0);
    G4LogicalVolume *logicBackFanout = new G4LogicalVolume(solidFanout, matBackFanout, "logicBackFanout", 0, 0, 0);
    G4LogicalVolume *logicAl = new G4LogicalVolume(solidAl, Al, "logicAl", 0, 0, 0);

    //Sensors for the tracker and calorimeter
    G4LogicalVolume *logicCalSensor = new G4LogicalVolume(solidSensor, Si, "logicCalSensor", 0, 0, 0);
    G4LogicalVolume *logicCalStrip = new G4LogicalVolume(solidSensorStrip, Si, "logicCalStrip", 0, 0, 0);
    fLogicCalPad = new G4LogicalVolume(solidSensorPad, Si, "logicCalPad", 0, 0, 0);

    // Subdivide Sensor logic volume into 4 sector Strips
    new G4PVReplica("SiCalStrip", logicCalStrip, logicCalSensor, kPhi, 4, 7.5*deg, 75.*deg);
    //Subdivide SensorStrip logic volume into 64 pad.
    new G4PVReplica("SiCalPad", fLogicCalPad, logicCalStrip, kRho, 64, 1.8*mm, 80.*mm);

    G4LogicalVolume *logicTr1Sensor = new G4LogicalVolume(solidSensor, Si, "logicTr1Sensor", 0, 0, 0);
    G4LogicalVolume *logicTr1Strip = new G4LogicalVolume(solidSensorStrip, Si, "logicTr1Strip", 0, 0, 0);
    fLogicTr1Pad = new G4LogicalVolume(solidSensorPad, Si, "logicTr1Pad", 0, 0, 0);

    // Subdivide Sensor logic volume into 4 sector Strips
    new G4PVReplica("SiTr1Strip", logicTr1Strip, logicTr1Sensor, kPhi, 4, 7.5*deg, 75.*deg);
    //Subdivide SensorStrip logic volume into 64 pad.
    new G4PVReplica("SiTr1Pad", fLogicTr1Pad, logicTr1Strip, kRho, 64, 1.8*mm, 80.*mm);

    G4LogicalVolume *logicTr2Sensor = new G4LogicalVolume(solidSensor, Si, "logicTr2Sensor", 0, 0, 0);
    G4LogicalVolume *logicTr2Strip = new G4LogicalVolume(solidSensorStrip, Si, "logicTr2Strip", 0, 0, 0);
    fLogicTr2Pad = new G4LogicalVolume(solidSensorPad, Si, "logicTr2Pad", 0, 0, 0);

    // Subdivide Sensor logic volume into 4 sector Strips
    new G4PVReplica("SiTr2Strip", logicTr2Strip, logicTr2Sensor, kPhi, 4, 7.5*deg, 75.*deg);
    //Subdivide SensorStrip logic volume into 64 pad.
    new G4PVReplica("SiTr2Pad", fLogicTr2Pad, logicTr2Strip, kRho, 64, 1.8*mm, 80.*mm);

    //***Place logical volumes***//
    //Origin of the world at the exit from colimator
    G4VPhysicalVolume *physicWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, 1);
    // Distances are taken from the logBook drawing (see ReadMe)
    // Place scintilators
    new G4PVPlacement(0, G4ThreeVector(0., 0., (450.+2.064)*mm), fLogicSc1, "Sc1", logicWorld, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (3607.+2.064)*mm), fLogicSc2, "Sc2", logicWorld, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (4607.+14.)*mm), fLogicSc3, "Sc3", logicWorld, false, 0, 1);
    // Construct MIMOSA26 plane from Si and Kapton volumes
    new G4PVPlacement(0, G4ThreeVector(), logicMimosaSi, "logicMimosaSi", logicMimosa26, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., -0.0375*mm), logicMimosaKapton, "logicMimosaKapton", logicMimosa26, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., 0.0375*mm), logicMimosaKapton, "logicMimosaKapton", logicMimosa26, false, 1, 1);
    // Place MIMOSA26 planes
    new G4PVPlacement(0, G4ThreeVector(0., 0., (471.+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (623.5+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 1, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (774.5+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 2, 1);

    new G4PVPlacement(0, G4ThreeVector(0., 0., (3477.+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 3, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (3528.+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 4, 1);
    new G4PVPlacement(0, G4ThreeVector(0., 0., (3578.+0.05)*mm), logicMimosa26, "logicMimosa", logicWorld, false, 5, 1);

#ifdef RUN_PH
    //Place target
    new G4PVPlacement(0, G4ThreeVector(0., 0., 1351.*mm), logicTarget, "logicTarget", logicWorld, false, 0, 1);
    // //Place magnet
    new G4PVPlacement(0, G4ThreeVector(0., 0., 2118.*mm), fLogicMagnet, "logicMagnet", logicWorld, false, 0, 1);
#endif

    // Construct sensor mounted in carbon fiber
    G4double yPos = -(80. + 0.5 * (195.2 - 80.))*mm;
    G4double zFanoutFrontPos = (-0.395 + 0.075)*mm;
    G4double zAlFrontPos = zFanoutFrontPos + 0.075*mm + 0.01*mm;
    G4double zSiPos = zAlFrontPos + 0.01*mm + 0.16*mm;
    G4double zAlBackPos = zSiPos + 0.16*mm + 0.01*mm;
    G4double zFanoutBackPos = zAlBackPos + 0.01*mm + 0.075*mm;
    //For calorimeter
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutFrontPos), logicFrontFanout, "CalFanoutFront", logicCalCarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlFrontPos), logicAl, "CalAlFront", logicCalCarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zSiPos), logicCalSensor, "SiCalSensor", logicCalCarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlBackPos), logicAl, "CalAlBack", logicCalCarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutBackPos), logicBackFanout, "CalFanoutBack", logicCalCarbonFiber, false, 0, 1);

    //For trackers
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutFrontPos), logicFrontFanout, "Tr1FanoutFront", logicTr1CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlFrontPos), logicAl, "Tr1AlFront", logicTr1CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zSiPos), logicTr1Sensor, "SiTr1Sensor", logicTr1CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlBackPos), logicAl, "Tr1AlBack", logicTr1CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutBackPos), logicBackFanout, "Tr1FanoutBack", logicTr1CarbonFiber, false, 0, 1);

    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutFrontPos), logicFrontFanout, "Tr2FanoutFront", logicTr2CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlFrontPos), logicAl, "Tr2AlFront", logicTr2CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zSiPos), logicTr2Sensor, "SiTr2Sensor", logicTr2CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlBackPos), logicAl, "Tr2AlBack", logicTr2CarbonFiber, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutBackPos), logicBackFanout, "Tr2FanoutBack", logicTr2CarbonFiber, false, 0, 1);



    //Place sensors and absorbers in the box slots
    //ySlotPos shifts detector down so electrons hit in the same area as 5 GeV electrons in the TB16 data
#ifdef RUN_PH
    G4double ySlotPos = -(168.3 - 80. - (195.2 - 80.) / 2.)*mm;
#else
    G4double ySlotPos = -(164.3 - 80. - (195.2 - 80.) / 2.)*mm;
#endif

    G4double zSlot = 4.5*mm;
    // misalignments of each plane is taken from Itamar simulation
    G4double misalignment[8] = {-0.11*mm, -1.26*mm, 0.46*mm, -0.275644*mm, 1.87705*mm, -1.2183*mm, 0.53323*mm, 0.*mm};
    // G4double misalignment[8] = {0.*mm};

    // 6.7 meters away from a collimator exit
    G4double zBoxPos = 6724*mm;

    // Calculated z positions of each slot in the Box
    G4double zSlotPos[29];
    for(G4int i=0; i<29; i++) zSlotPos[i] = zBoxPos + 0.5*zSlot + zSlot*i;

    //Place trackers planes in 1th and 6th slots of the box
    G4double zTr1Pos = zSlotPos[0] - 0.395*mm;
    G4double zTr2Pos = zSlotPos[5] - 0.395*mm;
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[0], zTr1Pos), logicTr1CarbonFiber, "Tr1CarbonFiber", logicWorld, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[1], zTr2Pos), logicTr2CarbonFiber, "Tr2CarbonFiber", logicWorld, false, 1, 1);



    G4double zSensorPos;
    G4double zAbsorberPos;
    // Place 3 MSG absorbers withot sensors in 21,22,23 slots
    for(G4int i=20; i<23; i++){
        zAbsorberPos = zSlotPos[i] + 1.785*mm;
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, zAbsorberPos), logicAbsorberMSG, "AbsorberMSG", logicWorld, false, 0, 1);
    }
    // Place 5 Sensors + PL absorbers in slots
    for(G4int i=23; i<28; i++){
        zSensorPos = zSlotPos[i] - 0.395*mm;
        zAbsorberPos = zSlotPos[i] + 1.75*mm;
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[i-21], zSensorPos), logicCalCarbonFiber, "CalCarbonFiber", logicWorld, false, i-21, 1);
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[i-21], zAbsorberPos), logicAbsorberPL, "AbsorberPL", logicWorld, false, 0, 1);
    }
    //Final TAB Sensor + MSG absorber (badly glued - not simulated)
    for(G4int i=28; i<29; i++){
        zSensorPos = zSlotPos[i] - 0.395*mm;
        zAbsorberPos = zSlotPos[i] + 1.785*mm;
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[i-21], zSensorPos), logicCalCarbonFiber, "CalCarbonFiber", logicWorld, false, i-21, 1);
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos + misalignment[i-21], zAbsorberPos), logicAbsorberMSG, "AbsorberMSG", logicWorld, false, 0, 1);
    }

    //***Make fancy visualisation***//
    logicWorld->SetVisAttributes(G4Color(0.79, 1.00, 0.90, 0.1));
    fLogicSc1->SetVisAttributes(G4Colour(0.2, 0., 0.2));
    fLogicSc2->SetVisAttributes(G4Colour(0.2, 0., 0.2));
    fLogicSc3->SetVisAttributes(G4Colour(0.2, 0., 0.2));
    logicMimosaSi->SetVisAttributes(G4VisAttributes::Invisible);
    logicMimosaKapton->SetVisAttributes(G4VisAttributes::Invisible);
    logicMimosa26->SetVisAttributes(G4Color(0.847, 0.835, 0.047));
    logicAbsorberPL->SetVisAttributes(G4Color(0.23, 0.48, 0.34));
    logicAbsorberMSG->SetVisAttributes(G4Color(0.25, 0.51, 0.43));
    logicCalCarbonFiber->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr1CarbonFiber->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr2CarbonFiber->SetVisAttributes(G4VisAttributes::Invisible);
    logicFrontFanout->SetVisAttributes(G4VisAttributes::Invisible);
    logicBackFanout->SetVisAttributes(G4VisAttributes::Invisible);
    logicAl->SetVisAttributes(G4VisAttributes::Invisible);
    logicCalSensor->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr1Sensor->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr2Sensor->SetVisAttributes(G4VisAttributes::Invisible);
    logicCalStrip->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr1Strip->SetVisAttributes(G4VisAttributes::Invisible);
    logicTr2Strip->SetVisAttributes(G4VisAttributes::Invisible);
    fLogicCalPad->SetVisAttributes(G4Color(0., 0., 1., 0.5));
    fLogicTr1Pad->SetVisAttributes(G4Color(0., 0., 1., 0.5));
    fLogicTr2Pad->SetVisAttributes(G4Color(0., 0., 1., 0.5));
    // std::cout<<"End of LCDetectorConstruction::Construct"<<std::endl;

    return physicWorld;
}

void LCDetectorConstruction::ConstructSDandField(){
    // std::cout<<"Start of LCDetectorConstruction::ConstructSDandField"<<std::endl;

    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    auto lumicalSD = new LCSD("LumicalSD", "LumicalHCName");
    G4SDManager::GetSDMpointer()->AddNewDetector(lumicalSD);
    fLogicCalPad->SetSensitiveDetector(lumicalSD);
    fLogicTr1Pad->SetSensitiveDetector(lumicalSD);
    fLogicTr2Pad->SetSensitiveDetector(lumicalSD);

    auto triggerSD = new LCSDTrigger("TriggerSD", "TriggerHCName");
    G4SDManager::GetSDMpointer()->AddNewDetector(triggerSD);
    fLogicSc1->SetSensitiveDetector(triggerSD);
    fLogicSc2->SetSensitiveDetector(triggerSD);
    fLogicSc3->SetSensitiveDetector(triggerSD);

#ifdef RUN_PH
    G4MagneticField* magField = new G4UniformMagField(G4ThreeVector(0.095*tesla, 0.*tesla, 0.*tesla));

    G4FieldManager* fieldManager = new G4FieldManager();
    fieldManager->SetDetectorField(magField);
    fieldManager->CreateChordFinder(magField);
    fLogicMagnet->SetFieldManager(fieldManager, true);

#endif
// std::cout<<"End of LCDetectorConstruction::ConstructSDandField"<<std::endl;

}
