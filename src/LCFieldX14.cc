#include "LCFieldX14.hh"
#include "Setup.hh"

#include "G4SystemOfUnits.hh"

LCFieldX14::LCFieldX14(G4String evfile){
    const char *fn = evfile.data();
    inputFile.open((char*)fn);
    if(inputFile){
        fileName = evfile;
        G4cout<<G4endl;
        G4cout<<"LCFieldX14::Initializing DID Field map from: "<<fileName<<G4endl; 
    }
    else{
        G4Exception("LCFieldX14:: cannot open file: ", evfile, RunMustBeAborted, " run aborted!");
    }
  
    G4String aLine; 
    while(getline(inputFile, aLine)){
        G4cout<<aLine<<G4endl;
        if(aLine.find("%") != 0 ) break;
    }

    G4int Nz, iz; // number of points in the map getline(inputFile, aline);
    G4double scale_factor = Setup::Nominal_Field_value / (3.5*tesla); // the map is for 3.5 tesla
    G4cout<<"The map is for 3.5[T] - scale factor used is: "<<scale_factor<<G4endl;

    sscanf(aLine, "%d", &Nz);
    G4cout<<"Found "<<Nz<<" points in the map"<<G4endl;
    for(iz=0; iz<Nz; iz++){
        inputFile>>zPos[iz]>>Bz[iz]>>Br[iz]>>Bx[iz];
        zPos[iz] *= cm;
        Bz[iz] *= scale_factor*kilogauss;
        Br[iz] *= scale_factor*kilogauss;
        Bx[iz] *= scale_factor*kilogauss;
    }
    
    inputFile.close();
    zStep = zPos[1] - zPos[0];
    xTan = tan(Setup::Beam_Crossing_Angle / 2. );
    rmax_sq = sqr(Setup::world_hdy);
    zmax = Setup::world_hdz;
    G4cout<<"LCFieldX14::DID Field map initialized in range: "<<-zPos[Nz-1]/mm<<" < Z < "<<zPos[Nz-1]/mm<<" [mm] "<<G4endl<<G4endl; 
}

LCFieldX14::~LCFieldX14(){;}

void LCFieldX14::GetFieldValue(const double Point[3], double *Bfield) const{
    if(std::abs(Point[2]) < zmax && (sqr(Point[0])+sqr(Point[1])) < rmax_sq){
        G4int iplow = (G4int)floor(fabs(Point[2] / zStep));
        const G4int signZ = (Point[2] >= 0) ? (+1) : (-1);
        G4double Dpoint = Point[2] * xTan;

        if(fabs(Dpoint) < 0.001) Dpoint = signZ * 0.001; 
        Bfield[0] = Point[0] * Br[iplow] / Dpoint + signZ * Bx[iplow];
        Bfield[1] = Point[1] * Br[iplow] / Dpoint;
        Bfield[2] = Bz[iplow];
    }
    else{
        Bfield[0] = 0.;
        Bfield[1] = 0.;
        Bfield[2] = 0.;
    }
}
