#include "LCDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

LCDetectorConstruction::LCDetectorConstruction()
    :logicWorld(0),
    physicWorld(0),
    logicSlot(0),
    logicAbsorber(0),
    logicSensor(0),
    logicFanoutFront(0),
    logicFanoutBack(0),
    logicSi(0),
    logicAl(0){}

LCDetectorConstruction::~LCDetectorConstruction(){}

G4VPhysicalVolume* LCDetectorConstruction::Construct(){
    buildEpoxy();

    logicWorld = buildWorld();
    logicSlot = buildSlot();
    logicSensor = buildSensor();
    logicAbsorber = buildAbsorber();
    logicFanoutFront = buildFanout("logicFanoutFront", 0.065*mm, 0.05*mm, 0.035*mm);
    logicFanoutBack = buildFanout("logicFanoutBack", 0.06*mm, 0.065*mm, 0.025*mm);
    logicSi = buildSi();
    logicAl = buildAl();
    logicSc2 = buildSc2();
    logicSc3 = buildSc3();

    fancyVisualization();

    // Place all Si, Al, Fanout volumes inside carbon fiber support (create sensor)
    G4double yPos = -(rSensorMin + 0.5*(rSensorMax - rSensorMin));
    G4double zFanoutFrontPos = -0.5*zSensor + 0.5*zFanout;
    G4double zAlFrontPos = zFanoutFrontPos + 0.5*zFanout + 0.5*zAl;
    G4double zSiPos = zAlFrontPos + 0.5*zAl + 0.5*zSi;
    G4double zAlBackPos = zSiPos + 0.5*zSi + 0.5*zAl;
    G4double zFanoutBackPos = zAlBackPos + 0.5*zAl + 0.5*zFanout;

    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutFrontPos), logicFanoutFront, "FanoutFront", logicSensor, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlFrontPos), logicAl, "AlFront", logicSensor, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zSiPos), logicSi, "Si", logicSensor, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zAlBackPos), logicAl, "AlBack", logicSensor, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., yPos, zFanoutBackPos), logicFanoutBack, "FanoutBack", logicSensor, false, 0, 1);

    // Place Sensor and Absorber into Slot mother volume
    G4double zAbsorberPos = -0.5*zSlot + 0.5*zAbsorber;
    G4double zSensorPos = 0.5*zSlot - 0.5*zSensor;

    // new G4PVPlacement(0, G4ThreeVector(0., 0., zAbsorberPos), logicAbsorber, "Absorber", logicSlot, false, 1);
    // new G4PVPlacement(0, G4ThreeVector(0., 0., zSensorPos), logicSensor, "Sensor", logicSlot, false, 0, 1);

    // Create world and place slots in it
    physicWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, 1);

    //ySlotPos is choosen to hit in the same area as 5 GeV electron on the TB16
    G4double ySlotPos = -(163.*mm - rSensorMin - (rSensorMax - rSensorMin) / 2.);
    G4double zBoxPos = 3300.*mm;
    G4double zSlotPos[40];
    for(G4int i=0; i<40; i++) zSlotPos[i] = zBoxPos + 0.5*zSlot + (zSlot + 0.002*mm)*i;

    G4double tr1Pos = zSlotPos[0]-0.5*zSlot+1.*mm-0.5*zSensor;
    G4double tr2Pos = zSlotPos[5]-0.5*zSlot+1.*mm-0.5*zSensor;

    // Trigger scintilators
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, 20.*mm), logicSc2, "Sc2", logicWorld, false, 0, 1);
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, 1130.*mm), logicSc3, "Sc3", logicWorld, false, 0, 1);

    // Place Trackers in Slots
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, tr1Pos), logicSensor, "Sensor", logicWorld, false, 1, 1);
    new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, tr2Pos), logicSensor, "Sensor", logicWorld, false, 6, 1);

    for(int i=0; i<20; i++){
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, zSlotPos[20+i] + zAbsorberPos), logicAbsorber, "Absorber", logicWorld, false, 0, 1);
        new G4PVPlacement(0, G4ThreeVector(0., ySlotPos, zSlotPos[20+i] + zSensorPos), logicSensor, "Sensor", logicWorld, false, 21+i, 1);
    }


    // Create sensetive detector
    G4SDManager *SDmanager = G4SDManager::GetSDMpointer();

    G4double cellRho = (rSensorMax - rSensorMin - 2.*rSensorGap)/64.; // 1.8 mm
    G4double phiSector = 7.5*deg;
    sensetiveDetector = new LCSensitiveDetector("LumiCalSD", "LCHitsCollectionName", rSensorMin, cellRho, phiSector, 64);

    // Cells are the sensitive detectors
    SDmanager->AddNewDetector(sensetiveDetector);
   
    logicSi->SetSensitiveDetector(sensetiveDetector);

    return physicWorld;
}

G4LogicalVolume *LCDetectorConstruction::buildWorld(){
    G4Material *Air = materials->FindOrBuildMaterial("G4_AIR");
    G4Box *solidWorld = new G4Box("World", 1200.*mm, 1200.*mm, 10000.*mm);
    return new G4LogicalVolume(solidWorld, Air, "World", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildSlot(){
    G4Material *Air = materials->FindOrBuildMaterial("G4_AIR");
    G4Box *solidSlot = new G4Box("solidSlot", 150.0*mm, 150.0*mm, 0.5*zSlot);
    return new G4LogicalVolume(solidSlot, Air, "logicSlot", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildAbsorber(){
    // For simulation PL absorber is used (W:95%, Ni:2.5%, Cu:2.5%)    
    G4Material *W = materials->FindOrBuildMaterial("G4_W");
    G4Material *Ni = materials->FindOrBuildMaterial("G4_Ni");
    G4Material *Cu = materials->FindOrBuildMaterial("G4_Cu");

    G4Material *matAbsorber = new G4Material("matAbsorber", 18.0*g/cm3, 3);
    matAbsorber->AddMaterial(W,0.95); 
    matAbsorber->AddMaterial(Ni,0.025); 
    matAbsorber->AddMaterial(Cu,0.025);  

    G4Box *solidAbsorber = new G4Box("solidAbsorber", 70.0*mm, 70.0*mm, 0.5*zAbsorber);
    return new G4LogicalVolume(solidAbsorber, matAbsorber, "logicAbsorber", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildSensor(){
    //This is carbon fiber support. Mother logic volum for sensor (Si, Al, Fanouts)
    G4Material *Carbon = materials->FindOrBuildMaterial("G4_C");
    G4Material *Epoxy = materials->FindOrBuildMaterial("Epoxy", true, true);

    G4Material *matSensor = new G4Material("matSensor", 1.6*g/cm3, 2);
    matSensor->AddMaterial(Carbon, 0.5);
    matSensor->AddMaterial(Epoxy, 0.5);

    G4Box *solidSensor = new G4Box("solidSensor", 70.0*mm, 70.0*mm, 0.5*zSensor);
    return new G4LogicalVolume(solidSensor, matSensor, "logicSensor", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildFanout(G4String logicName, G4double zEpoxy, G4double zKapton, G4double zCu){
    // G4double zFanout = zEpoxy + zKapton + zCu; defined in hearder. It should sum to 0.15*mm

    //Create Epoxy material

    G4Material *Epoxy = materials->FindOrBuildMaterial("Epoxy", true, true);
    G4Material *Kapton = materials->FindOrBuildMaterial("G4_KAPTON");
    G4Material *Cu = materials->FindOrBuildMaterial("G4_Cu");

    //Calculate Fanout density
    G4double densEpoxy = Epoxy->GetDensity();
    G4double densKapton = Kapton->GetDensity();
    G4double densCu = Cu->GetDensity();

    G4double densFanout;
    if(logicName == "logicFanoutFront") densFanout = (densEpoxy*zEpoxy/zFanout + densKapton*zKapton/zFanout + 0.5*densCu*zCu/zFanout);
    else densFanout = (densEpoxy*zEpoxy/zFanout + densKapton*zKapton/zFanout + densCu*zCu/zFanout);

    //Create Fanout material
    G4String matName;
    if(logicName == "logicFanoutFront") matName = "matFanoutFront";
    else matName = "matFanoutBack";
    G4Material *matFanout = new G4Material(matName, densFanout, 3);
    matFanout->AddMaterial(Epoxy, zEpoxy/zFanout);
    matFanout->AddMaterial(Kapton, zKapton/zFanout);
    matFanout->AddMaterial(Cu, zCu/zFanout);

    G4Tubs *solidFanout = new G4Tubs("solidFanout", rSensorMin, rSensorMax, 0.5*zFanout, 75.*deg, 30.*deg);

    return new G4LogicalVolume(solidFanout, matFanout, logicName, 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildSi(){
    G4Material *Si = materials->FindOrBuildMaterial("G4_Si");
    G4Tubs *solidSi = new G4Tubs("solidSi", rSensorMin+rSensorGap, rSensorMax-rSensorGap, 0.5*zSi, 75.*deg, 30.*deg);
    return new G4LogicalVolume(solidSi, Si, "logicSi", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildAl(){
    G4Material *Al = materials->FindOrBuildMaterial("G4_Al");
    G4Tubs *solidAl = new G4Tubs("solidAl", rSensorMin+rSensorGap, rSensorMax-rSensorGap, 0.5*zAl, 75.*deg, 30.*deg);
    return new G4LogicalVolume(solidAl, Al, "logicAl", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildSc2(){
    G4Box *solidSc2 = new G4Box("solidSc2", 150.*mm, 150.*mm, 3.75*mm);
    G4Material *matSc2 = materials->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    return new G4LogicalVolume (solidSc2, matSc2, "logicSc2", 0, 0, 0);
}

G4LogicalVolume *LCDetectorConstruction::buildSc3(){
    G4Box *solidSc3 = new G4Box("solidSc3", 150.*mm, 150.*mm, 10.0*mm);
    G4Material *matSc3 = materials->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    return new G4LogicalVolume (solidSc3, matSc3, "logicSc3", 0, 0, 0);
}

void LCDetectorConstruction::fancyVisualization(){
    G4VisAttributes* colorWorld = new G4VisAttributes(G4Color(1., 1., 1., 0.1));
    logicWorld->SetVisAttributes(colorWorld);

    logicSlot->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes* absColor = new G4VisAttributes(G4Color(0.5, 0.5, 0.5));
    logicAbsorber->SetVisAttributes(absColor);

    logicSensor->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes* sensorColor = new G4VisAttributes(G4Color(0.5, 0.5, 0.));
    logicSi->SetVisAttributes(sensorColor);

    logicFanoutFront->SetVisAttributes(G4VisAttributes::Invisible);
    logicFanoutBack->SetVisAttributes(G4VisAttributes::Invisible);
    logicAl->SetVisAttributes(G4VisAttributes::Invisible);

    logicSc2->SetVisAttributes(G4Colour(1., 0., 1.));
    logicSc3->SetVisAttributes(G4Colour(1., 0., 1.));
}

void LCDetectorConstruction::buildEpoxy(){
    G4Element *H = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element *C = new G4Element("Carbon", "C", 6., 12.01*g/mole);
    G4Element *O = new G4Element("Oxygen", "O", 8., 16.00*g/mole);

    G4Material *Epoxy = new G4Material("Epoxy", 1.3*g/cm3, 3);
    Epoxy->AddElement(H, 0.1310);
    Epoxy->AddElement(C, 0.5357);
    Epoxy->AddElement(O, 0.3333);
}


