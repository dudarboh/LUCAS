"""
Class to setup initial parameters for Lcal geometry, materials, and I/O.
"""
// getopt()
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4NistManager.hh"
#include "G4ios.hh"

#include "Setup.hh"

#include "G4SystemOfUnits.hh"

// default values for all setup parameters
// for world
Setup* Setup::theSetup = NULL;
clock_t Setup::StartTime;
G4int   Setup::LogFreq = 10;
G4double Setup::world_hdx = 1000.*mm;
G4double Setup::world_hdy = 1000.*mm;
G4double Setup::world_hdz = 10000.*mm;

// for globals
G4long   Setup::StartRandomSeed;
G4int    Setup::EventStartNumber = 0;
G4int    Setup::MaxStepsNumber = 10;
G4double Setup::StepSizeMin = 0.;

// Variables that can be specified through command line arguments
G4bool   Setup::batchMode = false;
G4int    Setup::PrintLevel = 0;
G4String Setup::macroName = "";
G4String Setup::RootFileName = "";
G4String Setup::RootFileMode = "";
G4bool   Setup::AccumulateEvents = false;
G4double Setup::rangeCut = 0.005*mm;
G4String Setup::SetupFile = "";

G4double Setup::Nominal_Field_value = 3.5 *tesla;
G4String Setup::Particle_Generator = "";
G4int    Setup::NoOfEventsToProcess = 0;
G4int    Setup::LcalTBeam = 0;
G4String Setup::Build_Beampipe = "Yes";
G4String Setup::Build_LCal = "Yes";
G4String Setup::Build_LHcal = "Yes";
G4String Setup::Build_BCal = "Yes";
G4String Setup::Build_Mask ="Yes";
G4double Setup::LCal_Region_Cut =  0.005;
G4double Setup::LHcal_Region_Cut = 1.;
G4double Setup::BCal_Region_Cut  = 1.;
G4double Setup::Mask_Region_Cut  = 1.;
G4String Setup::TBeam_senrio = "APS APS APS APS APS APS APS APS APS APS APS APS";

//   for beam pipe
G4double Setup::Beam_pipe_thickness = 1.0 *mm; 
G4double Setup::Beam_pipe_zend = 3500. *mm; 
G4double Setup::Lcal_to_BeamPipe_clearance = 1.0 *mm; 
G4int    Setup::Beam_pipe_VisSolid = 1 ;
G4int    Setup::LHcal_VisSolid     = 1 ;
G4int    Setup::BCal_VisSolid      = 1 ;
G4int    Setup::Mask_VisSolid      = 1 ;

// for LCAL
// base LCAL
G4bool   Setup::Lcal_virtual_cells = true;
G4bool   Setup::Lcal_layer_fan = false;
G4int    Setup::Lcal_n_layers   = 30;
G4int    Setup::Lcal_n_tiles    = 12;
G4int    Setup::Lcal_n_sectors  = 48;
G4int    Setup::Lcal_n_rings    = 64;

G4double Setup::Lcal_z_end = 2635. *mm;
G4double Setup::Lcal_inner_radius =  76.0 *mm;
G4double Setup::Lcal_outer_radius = 224.5 *mm;
G4double Setup::Lcal_SensRadMin   =  80.0 *mm;
G4double Setup::Lcal_SensRadMax   = 195.2 *mm;
G4double Setup::Lcal_layers_phi_offset = 3.75 *deg;
G4double Setup::Lcal_Phi_Offset = -3.75 *deg;
G4double Setup::Lcal_start_phi = 0. *deg;
G4double Setup::Lcal_end_phi = 360. *deg;
 
G4double Setup::Lcal_space_for_ears    = 25.5 *mm;
G4double Setup::Lcal_sector_dead_gap    = 1.2 *mm;
G4double Setup::Lcal_layer_gap          = 0.2  *mm;
G4double Setup::Lcal_silicon_thickness  = 0.32 *mm;
G4double Setup::Lcal_pad_metal_thickness= 0.02 *mm;
G4double Setup::Lcal_tungsten_thickness = 3.5  *mm;
G4double Setup::Lcal_absorber_density   =19.3 *g/cm3;
G4double Setup::Lcal_absorber_pitch = 1  *mm;

G4double Setup::Lcal_ChipCaveDepth =   2.6  *mm;
G4double Setup::Lcal_FEChip_space  =   0.5  *mm;
G4double Setup::Lcal_FEChip_rmax   = 250.0  *mm;
G4double Setup::Lcal_FEChip_rmin   = 195.2  *mm;
G4double Setup::Lcal_PCB_thickness =   0.9  *mm;

G4int    Setup::Lcal_support      = 1 ;
G4int    Setup::Lcal_use_absorber = 1 ;
G4int    Setup::Lcal_use_fanout   = 1 ;
G4int    Setup::Lcal_use_FE       = 1 ;
G4int    Setup::Lcal_VisSensSolid = 0;
G4int    Setup::Lcal_VisAbsSolid  = 0;

// base fanout

// epoxy
////--------------------------------------------
////            original value : 
////-------------------------------------------
// G4double Setup::Lcal_epoxy_heightF  = 0.075*mm;
// G4double Setup::Lcal_epoxy_heightB  = 0.150*mm;   // 2 layers * 75um each
// G4double Setup::Lcal_epoxy_propF    = 0.5;        // points of epoxy on sensor
// G4double Setup::Lcal_epoxy_propB    = 0.5;

// kapton
// G4double Setup::Lcal_kapton_heightF = 0.050*mm;
// G4double Setup::Lcal_kapton_heightB = 0.050*mm;
// G4double Setup::Lcal_kapton_prop    = 1.;

// copper - only on front fanout
// G4double Setup::Lcal_copper_heightF = 0.035*mm;
// G4double Setup::Lcal_copper_heightB = 0.035*mm;
// G4double Setup::Lcal_copper_propF   = 0.5; 
// G4double Setup::Lcal_copper_propB   = 1.0;  

////            2016 value : 
G4double Setup::Lcal_epoxy_heightF  = 0.065*mm;
G4double Setup::Lcal_epoxy_heightB  = 0.060*mm;   // 2 layers * 75um each
G4double Setup::Lcal_epoxy_propF    = 1.0;        // points of epoxy on sensor
G4double Setup::Lcal_epoxy_propB    = 1.0;

// kapton
G4double Setup::Lcal_kapton_heightF = 0.050*mm;
G4double Setup::Lcal_kapton_heightB = 0.065*mm;
G4double Setup::Lcal_kapton_prop    = 1.;

// copper - only on front fanout
G4double Setup::Lcal_copper_heightF = 0.035*mm;
G4double Setup::Lcal_copper_heightB = 0.025*mm;
G4double Setup::Lcal_copper_propF   = 0.5; 
G4double Setup::Lcal_copper_propB   = 1.0;

// derived        
G4double Setup::Lcal_sens_Z0 = 0. ;
G4double Setup::Lcal_hdz = 0. ;
G4double Setup::Lcal_absorber_gap =0. ;
G4double Setup::Lcal_Cell0_radius = 0. ;
G4double Setup::Lcal_CellPitch = 0. ;
G4double Setup::Lcal_sensor_dz = 0. ;
G4double Setup::Lcal_layer_hdz = 0. ;
G4double Setup::Lcal_silicon_hdz = 0. ;
G4double Setup::Lcal_sector_dphi = 0.;
G4double Setup::Lcal_fanoutF_thickness = 0. ;
G4double Setup::Lcal_fanoutB_thickness = 0. ;
G4double Setup::Lcal_fanoutF_hdz = 0. ;
G4double Setup::Lcal_fanoutB_hdz = 0. ;
G4double Setup::Lcal_tungsten_hdz = 0. ;
G4double Setup::Lcal_surface_area = 0.;

// materials
G4Material* Setup::Vacuum = NULL;  
G4Material* Setup::Air = NULL;  
G4Material* Setup::PLASTIC_SC = NULL; 
G4Material* Setup::Alu = NULL;  
G4Material* Setup::Silicon = NULL;  
G4Material* Setup::Tungsten = NULL;  
G4Material* Setup::Iron = NULL;  
G4Material* Setup::Copper = NULL;  
G4Material* Setup::Beryllium = NULL;  
G4Material* Setup::Graphite = NULL;
G4Material* Setup::Carbon = NULL;
G4Material* Setup::Kapton = NULL;  
G4Material* Setup::Epoxy = NULL;  
G4Material* Setup::FanoutMatF = NULL;  
G4Material* Setup::FanoutMatB = NULL; 
G4Material* Setup::FR4 = NULL; 
G4Material* Setup::Wabsorber=NULL;
G4Material* Setup::Wabsorber_MGS=NULL;
G4Material* Setup::Wabsorber_PL=NULL;
G4Material* Setup::C_fiber= NULL;

Setup* Setup::GetSetup(){
    if(theSetup == 0) theSetup = new Setup();
    return theSetup;
}

Setup::Setup(){}
Setup::~Setup(){
    theSetup = 0;
    G4cout<<"Setup deleted ..."<<G4endl;
}

//-----------------------------------------------------------
void Setup::Usage(){
    G4cout
        << "Help: "<<"[options]\n"
        << "\n"
        << "-h              print this help message.\n"
        << "-b              batch mode \n"
        << "-m <filename>   specifies a macro file to be executed before running.\n"
        << "\n"
        << "-o <filename>   specifies file name for ROOT output.\n"
        << "-M <mode>       specifies ROOT file opening mode (default is CREATE \n"
        << "                to avoid accidental file overwriting).\n"
        << "                Possible values are RECREATE/UPDATE \n"
        << "-A              accumulate events from entire Run are to be\n"
        << "                written in one event (suitable only for beam background data)\n"
        << "\n"
        << "-c <double>     specifies the Geant 4 production range cut in mm.\n"
        << "                (default is " << Setup::rangeCut / mm << " mm)\n"
        << "-s <filename>   specifies name of the file with geometry setup \n"
        << "-P <int>        specifies printout level (default is 0= minimum print\n"
        << "                                                      3= debug printout\n"
        << G4endl;
    exit(EXIT_FAILURE);
}

void Setup::SetupInit(int argc, char *argv[]){
    time_t now = time(NULL);
    G4cout<<"Setup::SetupInit is started: "<<ctime(&now)<<G4endl;

    const int option;
    while((option=getopt(argc, argv, ":A:hx:c:ibm:M:o:P:s:")) != -1){
        switch(option){
            case 'h':
                Usage();
                break;
            case 'P':
                Setup::PrintLevel = std::atoi(optarg);
                break;
            case 'b':
                Setup::batchMode = true;
                break;
            case 'm':
                Setup::macroName = optarg;
                break;
            case 'o':
                Setup::RootFileName = optarg;
                break;
            case 'M':
                Setup::RootFileMode = optarg;
                break;
            case 'A':
                Setup::AccumulateEvents = true;
                break;
            case 'c':
                Setup::rangeCut = std::strtod(optarg, NULL)*mm;
                break;
            case 's':
                Setup::SetupFile = optarg;
                break;
            case ':':
                G4cout<<"Option -"<<char(optopt)<<" requires argument. \n"<<G4endl;
                break;
            case '?':
                G4cout<<"Uknown option -"<<char(optopt)<<"\n"<<G4endl;
                break;
        }
    }

    if(Setup::SetupFile != "") SetBaseParameters();
    else G4cout<<"Setup::SetupInit() : setting default geometry parameters "<<G4endl;
    SetDerivedParameters();
    AddMaterials();
}

void Setup::SetBaseParameters(){
    std::ifstream inputFile;
    inputFile.open(Setup::SetupFile);
    if(!inputFile.is_open()) G4Exception("Setup:: cannot open file: ", Setup::SetupFile, RunMustBeAborted, " Aborting !");

    G4String aLine;
    char parName[80] ;
    char sDum[80];
    int iDum;
    std::string ssDum;

    G4cout<<"----- Setup::SetBaseParameters(): initializing from file : " << Setup::SetupFile<<G4endl;
    if(Setup::PrintLevel == 1) G4cout<<"----- File content: "<<G4endl;
    while(getline(inputFile, aLine)){
        if(aLine.find("#") == 0) continue; //skip comments
        if(Setup::PrintLevel == 1) G4cout<<aLine<<G4endl;
        sscanf(aLine, "%s %s %s", parName, sDum, sDum);
    
        if(!strcmp(parName, "batchMode")){
            sscanf(aLine, "%s%d%s", parName, &iDum , sDum);
            if(iDum != 0) Setup::batchMode = true;
            if(Setup::macroName == "") Setup::macroName = sDum;
        }
    
    // globals   
    else if(!strcmp(parName, "EventStartNumber"))   sscanf(aLine, "%s %d", sDum, &(Setup::EventStartNumber)); 
    else if(!strcmp(parName, "LogFreq"))            sscanf(aLine, "%s %d", sDum, &(Setup::LogFreq)); 
    else if(!strcmp(parName, "MaxStepsNumber"))     sscanf(aLine, "%s %d", sDum, &(Setup::MaxStepsNumber)); 
    else if(!strcmp(parName, "StepSizeMin"))     sscanf(aLine, "%s %lf", sDum, &(Setup::StepSizeMin)); 
    else if(!strcmp(parName, "PrintLevel"))  G4cout<<" parameter PrintLevel in Setup file ignored "<<G4endl; 
    else if(!strcmp(parName, "RootFileName")) { if(Setup::RootFileName == ""){
                                                     sscanf(aLine, "%s %s", sDum, sDum);  Setup::RootFileName = sDum;}}
    else if(!strcmp(parName, "RootFileMode"))    {if(Setup::RootFileMode == ""){
                                                     sscanf(aLine, "%s %s", sDum, sDum); Setup::RootFileMode = sDum;}}
    else if(!strcmp(parName, "AccumulateEvents")) {
                                                     sscanf(aLine, "%s %d", sDum, &iDum); 
                                                         if(iDum == 1) Setup::AccumulateEvents = true;}
    else if(!strcmp(parName, "Build_Beampipe")) { sscanf(aLine, "%s %s", sDum, sDum); Setup::Build_Beampipe = sDum; }
    else if(!strcmp(parName, "Build_LHcal"))    { sscanf(aLine, "%s %s", sDum, sDum); Setup::Build_LHcal = sDum; } 
    else if(!strcmp(parName, "Build_BCal"))     { sscanf(aLine, "%s %s", sDum, sDum); Setup::Build_BCal = sDum; } 
    else if(!strcmp(parName, "Build_Mask"))     { sscanf(aLine, "%s %s", sDum, sDum); Setup::Build_Mask = sDum; } 
    else if(!strcmp(parName, "rangeCut")) {if(Setup::rangeCut == 0.005) sscanf(aLine, "%s %lf", sDum, &(Setup::rangeCut));}
    else if(!strcmp(parName, "LCal_Region_Cut")) {if(Setup::LCal_Region_Cut == 0.005)
                                                   sscanf(aLine, "%s %lf", sDum, &(Setup::LCal_Region_Cut));}
    else if(!strcmp(parName, "BCal_Region_Cut")) {if(Setup::BCal_Region_Cut == 1.000)
                                                   sscanf(aLine, "%s %lf", sDum, &(Setup::BCal_Region_Cut));}
    else if(!strcmp(parName, "LHcal_Region_Cut")) {if(Setup::LHcal_Region_Cut == 1.000)
                                                   sscanf(aLine, "%s %lf", sDum, &(Setup::LHcal_Region_Cut));}
    else if(!strcmp(parName, "Mask_Region_Cut")) {if(Setup::Mask_Region_Cut == 1.000)
                                                   sscanf(aLine, "%s %lf", sDum, &(Setup::Mask_Region_Cut));}
    else if(!strcmp(parName, "Beam_Crossing_Angle")) { 
                                              sscanf(aLine, "%s %lf", sDum, &(Setup::Beam_Crossing_Angle)) ;
                                              Setup::lorentzTransAngle = Setup::Beam_Crossing_Angle / 2. *mrad;
                                              Setup::Beam_Crossing_Angle *= mrad;
        }
    else if(!strcmp(parName, "Nominal_field_value")) { sscanf(aLine, "%s %lf", sDum, &(Setup::Nominal_Field_value)); 
                                                         Setup::Nominal_Field_value *= tesla;
    }
      // world
    else if(!strcmp(parName, "world_hdx")) sscanf(aLine, "%s %lf", sDum, &(Setup::world_hdx));  
    else if(!strcmp(parName, "world_hdy")) sscanf(aLine, "%s %lf", sDum, &(Setup::world_hdy));  
    else if(!strcmp(parName, "world_hdz")) sscanf(aLine, "%s %lf", sDum, &(Setup::world_hdz));
    //  Beam Pipe
    else if(!strcmp(parName, "Beam_pipe_thickness"))         sscanf(aLine, "%s %lf", sDum, &(Setup::Beam_pipe_thickness)); 
    else if(!strcmp(parName, "Beam_pipe_zend"))              sscanf(aLine, "%s %lf", sDum, &(Setup::Beam_pipe_zend)); 
    else if(!strcmp(parName, "Lcal_to_BeamPipe_clearance"))  sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_to_BeamPipe_clearance)); 
        else if(!strcmp(parName, "Beam_pipe_VisSolid"))          sscanf(aLine, "%s %d", sDum, &(Setup::Beam_pipe_VisSolid));
        else if(!strcmp(parName, "LHcal_VisSolid"))         sscanf(aLine, "%s %d", sDum, &(Setup::LHcal_VisSolid));
        else if(!strcmp(parName, "BCal_VisSolid"))          sscanf(aLine, "%s %d", sDum, &(Setup::BCal_VisSolid));
        else if(!strcmp(parName, "Mask_VisSolid"))          sscanf(aLine, "%s %d", sDum, &(Setup::Mask_VisSolid));
          //  LCAL 
    else if(!strcmp(parName, "Lcal_virtual_cells"    )){sscanf(aLine, "%s %d", sDum, &iDum); 
      if(iDum == 1) Setup::Lcal_virtual_cells = true;
      else Setup::Lcal_virtual_cells = false; }
        else if(!strcmp(parName, "LcalTBeam"))  sscanf(aLine, "%s %d", sDum, &(Setup::LcalTBeam)); // after change  LcalTBeam to int 
    //  else if(!strcmp(parName, "LcalTBeam")) { sscanf(aLine, "%s %d", sDum, &iDum); 
    //  if(iDum == 1) Setup::LcalTBeam = true; } 

    else if(!strcmp(parName, "Lcal_layer_fan"    )){sscanf(aLine, "%s %d", sDum, &iDum); if(iDum != 0) Setup::Lcal_layer_fan = true;}
    else if(!strcmp(parName, "Lcal_n_layers"    ))      sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_n_layers));
    else if(!strcmp(parName, "Lcal_n_tiles"   ))        sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_n_tiles)); 
    else if(!strcmp(parName, "Lcal_n_sectors"   ))      sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_n_sectors)); 
    else if(!strcmp(parName, "Lcal_n_rings"     ))      sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_n_rings)); 

        else if(!strcmp(parName, "Lcal_z_end"))             sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_z_end)); 
        else if(!strcmp(parName, "Lcal_inner_radius"))       sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_inner_radius)); 
        else if(!strcmp(parName, "Lcal_outer_radius"))       sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_outer_radius)); 
        else if(!strcmp(parName, "Lcal_SensRadMin"))         sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_SensRadMin)); 
        else if(!strcmp(parName, "Lcal_SensRadMax"))         sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_SensRadMax)); 
        else if(!strcmp(parName, "Lcal_space_for_ears"))     sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_space_for_ears)); 
    else if(!strcmp(parName, "Lcal_sector_dead_gap"  )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_sector_dead_gap));  
    else if(!strcmp(parName, "Lcal_layers_phi_offset")) { sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_layers_phi_offset));
                                                                                    Setup::Lcal_layers_phi_offset *= deg; } 
    else if(!strcmp(parName, "Lcal_start_phi")) { sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_start_phi));
                                                                                    Setup::Lcal_start_phi *= deg; } 
    else if(!strcmp(parName, "Lcal_end_phi")) { sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_end_phi));
                                                                                    Setup::Lcal_end_phi *= deg; } 
    else if(!strcmp(parName, "Lcal_Phi_Offset"  )) {
      sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_Phi_Offset));
      Setup::Lcal_Phi_Offset *= deg;
    } 
    else if(!strcmp(parName, "Lcal_layer_gap"))      sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_layer_gap )); 
    else if(!strcmp(parName, "Lcal_silicon_thickness")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_silicon_thickness)); 
    else if(!strcmp(parName, "Lcal_pad_metal_thickness")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_pad_metal_thickness)); 
    else if(!strcmp(parName, "Lcal_tungsten_thickness")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_tungsten_thickness)); 
    else if(!strcmp(parName, "Lcal_absorber_pitch")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_absorber_pitch)); 
    else if(!strcmp(parName, "Lcal_absorber_density")) {sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_absorber_density)); 
                                                Setup::Lcal_absorber_density *= g/cm3;}
                                                
    else if(!strcmp(parName, "Lcal_PCB_thickness")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_PCB_thickness)); 

    else if(!strcmp(parName, "Lcal_FEChip_space"))        sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_FEChip_space)); 
    else if(!strcmp(parName, "Lcal_FEChip_rmax"))        sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_FEChip_rmax)); 
    else if(!strcmp(parName, "Lcal_ChipCaveDepth"))      sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_ChipCaveDepth)); 

    else if(!strcmp(parName, "Lcal_use_absorber"))       sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_use_absorber));    
    else if(!strcmp(parName, "Lcal_support"))         sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_support));
    else if(!strcmp(parName, "Lcal_use_fanout"))         sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_use_fanout));
    else if(!strcmp(parName, "Lcal_use_FE"))         sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_use_FE));
    else if(!strcmp(parName, "Lcal_VisSensSolid"))       sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_VisSensSolid));
    else if(!strcmp(parName, "Lcal_VisAbsSolid"))        sscanf(aLine, "%s %d", sDum, &(Setup::Lcal_VisAbsSolid));
      //epoxy
    else if(!strcmp(parName, "Lcal_epoxy_heightF")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_epoxy_heightF)); 
    else if(!strcmp(parName, "Lcal_epoxy_heightB")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_epoxy_heightB)); 
    else if(!strcmp(parName, "Lcal_epoxy_propF"  )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_epoxy_propF));    
    else if(!strcmp(parName, "Lcal_epoxy_propB"  )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_epoxy_propB)); 
      // kapton
    else if(!strcmp(parName, "Lcal_kapton_heightF")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_kapton_heightF)); 
    else if(!strcmp(parName, "Lcal_kapton_heightB")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_kapton_heightB));  
    else if(!strcmp(parName, "Lcal_kapton_prop"  )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_kapton_prop));
      // copper   
    else if(!strcmp(parName, "Lcal_copper_heightF")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_copper_heightF)); 
    else if(!strcmp(parName, "Lcal_copper_heightB")) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_copper_heightB));  
    else if(!strcmp(parName, "Lcal_copper_propF" )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_copper_propF));  
    else if(!strcmp(parName, "Lcal_copper_propB" )) sscanf(aLine, "%s %lf", sDum, &(Setup::Lcal_copper_propB)); 
        else if(!strcmp(parName, "TBeam_senrio"      )) { Setup::TBeam_senrio = aLine.substr(aLine.find(" ")) ;}   
    else G4cout<<" ****** Unknown parameter name : " << parName<<G4endl;
  }
 
  inputFile.close();


}
 
void Setup::SetDerivedParameters(){
    // set obligatory defaults if nothing was set so far 
    if(Setup::Beam_Crossing_Angle < 0.) {  Setup::Beam_Crossing_Angle = 0. *mrad ;
                                          Setup::lorentzTransAngle = Setup::Beam_Crossing_Angle / 2. *mrad; } 
    if(Setup::RootFileMode == "")  Setup::RootFileMode = "CREATE"; 
    //
    G4double layer_thick;
    Setup::Lcal_silicon_hdz  = Setup::Lcal_silicon_thickness / 2.;
    Setup::Lcal_tungsten_hdz = Setup::Lcal_tungsten_thickness / 2.;
    Setup::Lcal_FEChip_rmin = Setup::Lcal_FEChip_rmax - Setup::Lcal_FEChip_space; 
    Setup::Lcal_CellPitch  = (  Setup::Lcal_SensRadMax 
                          -    Setup::Lcal_SensRadMin
              - 2.*Setup::Lcal_sector_dead_gap)/G4double(Setup::Lcal_n_rings);
    Setup::Lcal_Cell0_radius = Setup::Lcal_SensRadMin
                           + Setup::Lcal_sector_dead_gap + Setup::Lcal_CellPitch/2.;
    Setup::Lcal_surface_area = (sqr(Lcal_outer_radius)-sqr(Lcal_SensRadMin))*CLHEP::twopi / 2. ;
    // fanout
    Setup::Lcal_fanoutF_thickness = (Setup::Lcal_epoxy_heightF 
                                    + Setup::Lcal_kapton_heightF
                    + Setup::Lcal_copper_heightF) *mm;
    Setup::Lcal_fanoutF_hdz = Setup::Lcal_fanoutF_thickness / 2.;
    Setup::Lcal_fanoutB_thickness = (  Setup::Lcal_epoxy_heightB 
                                    + Setup::Lcal_kapton_heightB
                    + Setup::Lcal_copper_heightB) *mm;
    Setup::Lcal_fanoutB_hdz = Setup::Lcal_fanoutB_thickness / 2.;
    //
    if(Setup::Lcal_FEChip_rmin < Setup::Lcal_SensRadMax) {
    Setup::Lcal_absorber_gap = Setup::Lcal_silicon_thickness + Setup::Lcal_pad_metal_thickness 
                             + Setup::Lcal_PCB_thickness + Setup::Lcal_layer_gap;
    }
    else{
    Setup::Lcal_absorber_gap = Setup::Lcal_fanoutB_thickness + Setup::Lcal_silicon_thickness  
                             + Setup::Lcal_fanoutF_thickness + Setup::Lcal_pad_metal_thickness 
                             + Setup::Lcal_layer_gap;
    }
    layer_thick  =  Setup::Lcal_tungsten_thickness + Setup::Lcal_absorber_gap; 
    Setup::Lcal_layer_hdz = layer_thick / 2.;
    Setup::Lcal_hdz = G4double(Setup::Lcal_n_layers)*Setup::Lcal_layer_hdz;
    //  Setup::Lcal_absorber_density = Setup::Lcal_absorber_density  *g/cm3; // add to take care of density units 

    Setup::Lcal_sensor_dz = 2.*Setup::Lcal_layer_hdz;
                          
    Setup::Lcal_sector_dphi = Setup::Lcal_end_phi/ (G4double)Setup::Lcal_n_sectors;
    Setup::lorentzTransAngle= Setup::Beam_Crossing_Angle / 2.;
}

void Setup::AddMaterials(){

    //--------------- MATERIALS

    // some variables needed for defining my own materials
    G4double z, a, density, fractionmass;
    G4int ncomponents, natoms;
    G4String symbol;

    // Pre-defined materials (NIST)
    G4NistManager *materials = G4NistManager::Instance();
    // Get materials from the Geant4 database
    Setup::Silicon     = materials->FindOrBuildMaterial("G4_Si");
    Setup::Tungsten    = materials->FindOrBuildMaterial("G4_W");
    Setup::Iron        = materials->FindOrBuildMaterial("G4_Fe");
    Setup::Copper      = materials->FindOrBuildMaterial("G4_Cu");
    Setup::Alu         = materials->FindOrBuildMaterial("G4_Al");
    Setup::Kapton      = materials->FindOrBuildMaterial("G4_KAPTON");
    Setup::Beryllium   = materials->FindOrBuildMaterial("G4_Be");
    Setup::Graphite    = materials->FindOrBuildMaterial("G4_GRAPHITE");
    Setup::Carbon      = materials->FindOrBuildMaterial("G4_C");
    // Vacuum - low density air
    Setup::Air    = materials->FindOrBuildMaterial("G4_AIR");
    Setup::Vacuum = new G4Material("Vacuum", density= 1.7e-25*g/cm3, ncomponents=1);
    Setup::Vacuum->AddMaterial(Air, fractionmass=1);

    Setup::PLASTIC_SC    = materials->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
 
    // EPOXY - build up from elements
    G4Element* H  =
        new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
    G4Element* C  =
        new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
    G4Element* O  =
        new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
    G4Element* Si  =
        new G4Element("Silcone"  ,symbol="Si" , z= 14., a= 28.09*g/mole);
        Setup::Epoxy = new G4Material("Epoxy", density= 1.3*g/cm3, ncomponents=3);
        Setup::Epoxy->AddElement(H, fractionmass=0.1310);
        Setup::Epoxy->AddElement(C, fractionmass=0.5357);
        Setup::Epoxy->AddElement(O, fractionmass=0.3333);
    // fiber glass
    G4Material* fiberglass = new G4Material("fiberglass",density=2.61*g/cm3,ncomponents=2);
    fiberglass -> AddElement(Si, natoms=1);
    fiberglass -> AddElement(O, natoms=2);
    // PCBoard material FR4
    Setup::FR4 = new G4Material("FR4",density=1.85*g/cm3,ncomponents=2);
    Setup::FR4 ->AddMaterial(Setup::Epoxy, fractionmass=0.39);
    Setup::FR4 ->AddMaterial(fiberglass, fractionmass=0.61);
    // nickel
    //G4Element* Ni = new G4Element("Nickel",symbol="Ni", z=28, a=58.69348*g/mole);
    G4Material * Ni =materials->FindOrBuildMaterial("G4_Ni");
     //Setup::Silicon     = materials->FindOrBuildMaterial("G4_Si");
   // material for absorber
    Setup::Wabsorber = new G4Material("Wabsorber",Setup::Lcal_absorber_density, ncomponents=3);
    Setup::Wabsorber->AddMaterial(Setup::Tungsten, fractionmass=95.0*perCent); //93
    Setup::Wabsorber->AddMaterial(Ni, fractionmass=3.75*perCent); //5.25
    Setup::Wabsorber->AddMaterial(Setup::Copper, fractionmass=1.25*perCent); //1.75 
    // for the 2014 TB plate 
    Setup::Wabsorber_MGS = new G4Material("Wabsorber_MGS",17.7*g/cm3 , ncomponents=3);
    Setup::Wabsorber_MGS->AddMaterial(Setup::Tungsten, fractionmass=93.0*perCent); 
    Setup::Wabsorber_MGS->AddMaterial(Ni, fractionmass=5.25*perCent); 
    Setup::Wabsorber_MGS->AddMaterial(Setup::Copper, fractionmass=1.75*perCent); 
    // for the 2014 TB plate 
    Setup::Wabsorber_PL = new G4Material("Wabsorber_PL", 18.0*g/cm3 ,ncomponents=3);
    Setup::Wabsorber_PL->AddMaterial(Setup::Tungsten, fractionmass=95.0*perCent); 
    Setup::Wabsorber_PL->AddMaterial(Ni, fractionmass=2.5*perCent); 
    Setup::Wabsorber_PL->AddMaterial(Setup::Copper, fractionmass=2.5*perCent);  
    // for the 2016 TB cerry-on
    Setup::C_fiber = new G4Material("C_fiber", 1.6*g/cm3 ,ncomponents=2);
    Setup::C_fiber->AddMaterial(Setup::Carbon, fractionmass=50.0*perCent);
    Setup::C_fiber->AddMaterial(Setup::Epoxy, fractionmass=50.0*perCent);
    // FANOUT
    // fractional mass =
    // (density * volume occupied by the material)/total mass of the layer

    G4double epoxydens =  Setup::Epoxy -> GetDensity();
    G4double kaptondens = Setup::Kapton -> GetDensity();
    G4double copperdens = Setup::Copper -> GetDensity();
    //
    // Back fanout (ground) fractional masses 
    // 
    G4double epoxydensB= epoxydens *Setup::Lcal_epoxy_propB;
    G4double copperdensB = copperdens * Setup::Lcal_copper_propB;   
    G4double epoxyfracB  = Setup::Lcal_epoxy_heightB  / Lcal_fanoutB_thickness;
    G4double kaptonfracB = Setup::Lcal_kapton_heightB / Setup::Lcal_fanoutB_thickness;
    G4double copperfracB = Setup::Lcal_copper_heightB / Setup::Lcal_fanoutB_thickness;
    G4double backDensity = (2.* epoxydensB  * epoxyfracB    // two layers of epoxy
                               + kaptondens  * kaptonfracB
                       + copperdensB * copperfracB);
    Setup::FanoutMatB = new G4Material("BackFanoutMaterial",
                                backDensity,
                                ncomponents=3);
        Setup::FanoutMatB->AddMaterial(Setup::Copper, fractionmass=epoxyfracB);
        Setup::FanoutMatB->AddMaterial(Setup::Kapton, fractionmass=kaptonfracB);
        Setup::FanoutMatB->AddMaterial(Setup::Epoxy,  fractionmass=copperfracB);

    // Front fanout (PC) fractional masses 

    G4double epoxydensF  = epoxydens  * Setup::Lcal_epoxy_propF;
    G4double copperdensF = copperdens * Setup::Lcal_copper_propF;   
    G4double epoxyfracF  = Setup::Lcal_epoxy_heightF  / Setup::Lcal_fanoutF_thickness;
    G4double kaptonfracF = Setup::Lcal_kapton_heightF / Setup::Lcal_fanoutF_thickness;
    G4double copperfracF = Setup::Lcal_copper_heightF / Setup::Lcal_fanoutF_thickness;
    G4double frontDensity = ( epoxydensF  * epoxyfracF    // one layers of epoxy
                             + kaptondens  * kaptonfracF
                     + copperdensF * copperfracF);
 
    Setup::FanoutMatF = new G4Material("FrontFanoutMaterial",
                                frontDensity,
                                ncomponents=3);
        Setup::FanoutMatF->AddMaterial(Setup::Copper, fractionmass=epoxyfracF);
        Setup::FanoutMatF->AddMaterial(Setup::Kapton, fractionmass=kaptonfracF);
        Setup::FanoutMatF->AddMaterial(Setup::Epoxy,  fractionmass=copperfracF);
    //
    //  if(Setup::PrintLevel == 2){
    G4cout<<G4endl << "Setup::AddMaterials : New materials defined are: "<<G4endl<<G4endl;
    G4cout<<*(G4Material::GetMaterialTable())<<G4endl;
    //  }

}

