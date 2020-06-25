
#ifndef HPDetectorMessenger_h
#define HPDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class HPDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithABool;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HPDetectorMessenger: public G4UImessenger
{
public:

  HPDetectorMessenger(HPDetectorConstruction* );
  virtual ~HPDetectorMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:

  HPDetectorConstruction* Detector;

  G4UIdirectory*             detDir;
  G4UIcmdWithAString*        matCmd;
  G4UIcmdWithoutParameter*   updateCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

