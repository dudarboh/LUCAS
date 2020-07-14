I will re-do this later, if someone needs it. Below is the obsolete version, but still might be usefull.



Geant4 simulation of LumiCal test beam setup in 2016

1- GEOMETRY DEFINITION
 
![Test beam setup scheme](https://github.com/FoxWise/LUCAS/blob/master/TB2016setup.png)

![Test beam setup proportions](https://github.com/FoxWise/LUCAS/blob/master/TB2016setup_2.png)

    Geometry of the test beam in simulation is the following:
    - Primary particle source (gun) at the exit of colimator
    - 1st trigger scintilator
    - 3 MIMOSA Telescope planes
    - Cu target (only in photon_runs branch)
    - Magnet (only in photon_runs branch)
    - 3 MIMOSA Telescope planes
    - 2nd trigger scintilator
    - 3rd thick scintilator
    - Box with Lumical:
        -- Slot1 - Tracker1
        -- Slot6 - Tracker2
 	    -- Slot21,22,23 - 3 absorbers
        -- Slot24 - Sensor + absorber
        -- SLot25 - Sensor + absorber
        -- Slot26 - Sensor + absorber
        -- SLot27 - Sensor + absorber
        -- Slot28 - Sensor + absorber
        -- SLot29 - TAB glued Sensor + absorber (was badly glued in the test beam)

    -- Slot width - 4.5 mm
    -- Absorber thickness - 3.5 mm
    -- Sensor thicknes - +-0.8 mm

2- PHYSICS LIST
    QGSP_BERT
    Two different EM physics models mentioned by Sasha are commented in Lucas.cc
  
3- ACTION INITALIZATION

    Standart action initialization class to support multi-threading

4- PRIMARY GENERATOR

    GPS settings are tuned in startRun.mac macro.

    It defines particle type, energy, position, energy smearing, angular smearing, size 
 
5- RUNS and EVENTS

    Event class used only for printout to check speed of cimputation
    Run class is used for writing collected data by Senstivive detector in the root file
    In the end of each event.

    Run class also simulates electronic noise smearing.
    NOISE.TXT file is needed in ../ directory of compiled code
   
6- DETECTOR RESPONSE
      
    Energy deposited and IsPrimaryParticle are counted in sensors pads
    using sensitive detector.
    The physics quantities are stored in LCHit via LCSensitiveDetector
    objects. Sensitive detector is set to logical volume of the pad
  
9- HOW TO RUN
     
    If you are currently in the same directory as this file, then:

    mkdir ./build
    cd ./build
    cmake ../
    cmake --build .

    If you want to change install directory

    cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ../
    cmake --build . --target install
    cp ../noise.txt /path/to/

    Copy noise file one folder up. Or comment noise simulation in LCRun class

    To Run in Qt5 visualization mode:
    Change MACRO PATH in Lucas.cc file, then:
    ./Lucas
    (IT WILL NOT WORK TILL YOU CHANGE MACRO PATH IN Lucas.cc)
    Otherwise it will not find init_vis.mac

    To Run in batch mode:
    ./Lucas ../startRun.mac

    It will produce 2 output root files in the multi-threading mode. One of them is empty.
    Use only which has significat size
