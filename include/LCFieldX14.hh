//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#ifndef LCFieldX14_H
#define LCFieldX14_H 1

#include <fstream>
#include "globals.hh"
#include "G4MagneticField.hh"

class LCFieldX14 : public G4MagneticField
{
  public:
    LCFieldX14( G4String evfile);
    ~LCFieldX14();

    void GetFieldValue( const  double Point[3],
                               double *Bfield ) const;

  private:
    G4double rmax_sq;
    G4double zmax;
  G4String fileName;
  std::ifstream inputFile;
protected:
  G4double zStep,xTan,zPos[1000],Bz[1000],Br[1000],Bx[1000];
};

#endif

