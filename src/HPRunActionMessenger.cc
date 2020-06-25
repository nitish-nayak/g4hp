#include "HPRunActionMessenger.hh"
#include "HPRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "Randomize.hh"

HPRunActionMessenger::HPRunActionMessenger(HPRunAction* RA)
  :runAction (RA)
{
  
  runDir = new G4UIdirectory("/HP/run/");
  runDir->SetGuidance("Run control.");
  
  fileNameCmd = new G4UIcmdWithAString("/HP/run/outName",this);
  fileNameCmd->SetGuidance("get output file name from.");
  fileNameCmd->SetParameterName("fileName",true);
  fileNameCmd->SetDefaultValue ("fout_hp.root");
  fileNameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);  

}

HPRunActionMessenger::~HPRunActionMessenger()
{
   delete         runDir;
   delete         fileNameCmd;  
}

void HPRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if (command == fileNameCmd){ 
    runAction->SetOutputFileName(newValue);
  }
  
}
