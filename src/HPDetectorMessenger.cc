
#include "HPDetectorMessenger.hh"

#include "HPDetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

HPDetectorMessenger::HPDetectorMessenger(HPDetectorConstruction * Det)
:Detector(Det)
{
  detDir = new G4UIdirectory("/HP/det/");
  detDir->SetGuidance("Detector set.");

  matCmd = new G4UIcmdWithAString("/HP/det/TargetMat",this);
  matCmd->SetGuidance("Select Material for the target");
  matCmd->SetParameterName("Material",false);
  matCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  updateCmd = new G4UIcmdWithoutParameter("/HP/det/Update",this);
  updateCmd->SetGuidance("Update geometry.");
  updateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  updateCmd->SetGuidance("if you changed geometrical value(s)");
  updateCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

HPDetectorMessenger::~HPDetectorMessenger()
{
  delete matCmd;
  delete updateCmd;
  delete detDir;
}

void HPDetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if( command == matCmd )
   Detector->SetTargetMaterial(newValue);
  else if( command == updateCmd )
    Detector->UpdateGeometry();
}

