/* 13/05/2009.
 * adopted from Mokka Control class
 */
#ifndef SETUP_H
#define SETUP_H 1

#include <sys/times.h>

#include "globals.hh"
#include "G4Version.hh"
#define G4_VERSION_GE( VERSION ) (G4VERSION_NUMBER >= VERSION)

class G4Material;

class Setup{
    public:
        // Setup is singelton class. To get access to Setup
        // user must call Setup::GetSetup() static method
        static Setup *GetSetup();
        void SetupInit(int argc, char* argv[]);
        void SetBaseParameters();
        virtual ~Setup();

    //  globals
    public:
        static clock_t StartTime;
        static G4long StartRandomSeed;
        static G4int EventStartNumber;
        static G4int LogFreq;
        static G4int MaxStepsNumber;
        static G4double StepSizeMin;
        static G4int PrintLevel;
        static G4bool batchMode;
        static G4String macroName;
        static G4String RootFileName;
        static G4String RootFileMode;
        static G4String SetupFile;
        static G4double rangeCut;
        static G4double Beam_Crossing_Angle;
        static G4double lorentzTransAngle;
        static G4double Nominal_Field_value;
        static G4String Particle_Generator; 
        static G4int NoOfEventsToProcess;
        static G4bool AccumulateEvents;
        static G4String Build_Beampipe;
        static G4String Build_LCal;
        static G4String Build_LHcal;
        static G4String Build_BCal;
        static G4String Build_Mask;
        static G4double LCal_Region_Cut;
        static G4double BCal_Region_Cut;
        static G4double LHcal_Region_Cut;
        static G4double Mask_Region_Cut;

        // for World
        static G4double world_hdx;
        static G4double world_hdy;
        static G4double world_hdz;

        // for beampipe
        static G4double Beam_pipe_thickness; 
        static G4double Beam_pipe_zend;
        static G4double Lcal_to_BeamPipe_clearance;
        static G4int    Beam_pipe_VisSolid;
        // for LHcal
        static G4int LHcal_VisSolid;
        // for BCAL
        static G4int BCal_VisSolid;
        // for mask
        static G4int Mask_VisSolid;
     
    // for LCAL
    public:
        // base
        static G4int LcalTBeam;
        static G4bool Lcal_virtual_cells;
        static G4bool Lcal_layer_fan;
        static G4int Lcal_n_layers;
        static G4int Lcal_n_tiles;
        static G4int Lcal_n_sectors;
        static G4int Lcal_n_rings;

        static G4String TBeam_senrio; 
        static G4double Lcal_z_end;
        static G4double Lcal_inner_radius;
        static G4double Lcal_outer_radius;
        static G4double Lcal_SensRadMin;
        static G4double Lcal_SensRadMax;
        static G4double Lcal_layers_phi_offset;
        static G4double Lcal_Phi_Offset;
        static G4double Lcal_start_phi;
        static G4double Lcal_end_phi;

        static G4double Lcal_space_for_ears;
        static G4double Lcal_sector_dead_gap;
        static G4double Lcal_layer_gap;
        static G4double Lcal_silicon_thickness;
        static G4double Lcal_pad_metal_thickness;
        static G4double Lcal_tungsten_thickness;
        static G4double Lcal_absorber_density;
        static G4double Lcal_absorber_pitch;
        static G4int Lcal_support;
        static G4int Lcal_use_absorber;
        static G4int Lcal_use_fanout;
        static G4int Lcal_use_FE;
        static G4int Lcal_VisSensSolid;
        static G4int Lcal_VisAbsSolid;
        // FE chips space
        static G4double Lcal_ChipCaveDepth;
        static G4double Lcal_FEChip_space;
        static G4double Lcal_FEChip_rmax;
        static G4double Lcal_FEChip_rmin;
        static G4double Lcal_PCB_thickness;
        
        // FANOUT

        // epoxy
        static G4double Lcal_epoxy_heightF;
        static G4double Lcal_epoxy_heightB; // 2 layers * 100um each
        static G4double Lcal_epoxy_propF; // points of Lcal_epoxy on sensor
        static G4double Lcal_epoxy_propB;
        // kapton
        static G4double Lcal_kapton_heightF; // kapton thickness front
        static G4double Lcal_kapton_heightB; //   -       -      back
        static G4double Lcal_kapton_prop;
        // copper - only on front fanout. Why ? ask Jonathan
        static G4double Lcal_copper_heightF;
        static G4double Lcal_copper_heightB;
        static G4double Lcal_copper_propF; // Cu covers half the surface area
        static G4double Lcal_copper_propB; // Cu covers entire surface area

    // derived
    public:
        static G4double Lcal_sens_Z0; // z-pos of the first sensor
        static G4double Lcal_hdz; // half length of LCAL
        static G4double Lcal_Cell0_radius; // r-pos of the first cell
        static G4double Lcal_CellPitch;
        static G4double Lcal_sensor_dz; // distance between sensors
        static G4double Lcal_layer_hdz; // half thickness of the layer
        static G4double Lcal_silicon_hdz; // half thickness of the silicon
        static G4double Lcal_fanoutF_thickness;
        static G4double Lcal_fanoutB_thickness;
        static G4double Lcal_fanoutF_hdz; // half thickness fanout front
        static G4double Lcal_fanoutB_hdz; // half thickness fanout back 
        static G4double Lcal_tungsten_hdz; // half thickness absorber
        static G4double Lcal_sector_dphi;
        static G4double Lcal_surface_area;
        static G4double Lcal_absorber_gap;

    // materials
    public:
        static G4Material *Vacuum;  
        static G4Material *Air;  
        static G4Material *PLASTIC_SC;
        static G4Material *Silicon;  
        static G4Material *Alu;  
        static G4Material *Tungsten;  
        static G4Material *Copper;  
        static G4Material *Iron;  
        static G4Material *Beryllium;  
        static G4Material *Graphite; 
        static G4Material *Carbon;
        static G4Material *Kapton;  
        static G4Material *Epoxy;  
        static G4Material *FanoutMatF;  
        static G4Material *FanoutMatB;
        static G4Material *FR4;
        static G4Material *Wabsorber;
        static G4Material *Wabsorber_MGS;  // for 2014 TB plate 
        static G4Material *Wabsorber_PL;	// for 2014 TB plate	
        static G4Material *C_fiber;	// for 2015-6 TB 
    private:
        Setup(); 
        static Setup *theSetup;
        void SetDerivedParameters();
        void Usage();
        void AddMaterials();
};

#endif
