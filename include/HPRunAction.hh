
#ifndef HPRunAction_h
#define HPRunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "globals.hh"

class HPRunActionMessenger;

class HPRunAction : public G4UserRunAction
{
public:
  
  HPRunAction();
  ~HPRunAction();
  HPRunActionMessenger* raMessenger;
  
public: 
  
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);
  void SetOutputFileName(const G4String&);
  
  G4String fileName;
  
};

#endif

