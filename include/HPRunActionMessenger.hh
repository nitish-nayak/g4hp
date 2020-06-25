#ifndef HPRunActionMessenger_h
#define HPRunActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4ios.hh"

class HPRunAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class HPRunActionMessenger: public G4UImessenger
{
public:
  HPRunActionMessenger(HPRunAction* );
  ~HPRunActionMessenger();
  
  void SetNewValue(G4UIcommand* ,G4String );
 
private:

   HPRunAction*         runAction;
   G4UIdirectory*       runDir;
   G4UIcmdWithAString*  fileNameCmd;  
         
};

#endif
