
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "Randomize.hh"

#include "HPDetectorConstruction.hh"

#include "FTFP_BERT.hh"
#include "QGSP_BERT.hh"

#include "HPPrimaryGeneratorAction.hh"

#include "HPRunAction.hh"
#include "HPEventAction.hh"
#include "HPStackingAction.hh"
#include "HPTrackingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {

  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());

  //Construct the default run manager
  G4RunManager * runManager = new G4RunManager();

  //set mandatory initialization classes
  runManager->SetUserInitialization(new HPDetectorConstruction());
  
  FTFP_BERT* physicsList = new FTFP_BERT;
 
  runManager->SetUserInitialization(physicsList);
 
  runManager->SetUserAction(new HPPrimaryGeneratorAction());
 
  //set user action classes
  runManager->SetUserAction(new HPRunAction());
  runManager->SetUserAction(new HPEventAction());
  runManager->SetUserAction(new HPStackingAction());
  runManager->SetUserAction(new HPTrackingAction());
  
  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();
  
  G4String command = "/control/execute ";
  G4String fileName = argv[1];
  G4cout << "Executing " << command+fileName << G4endl;
  UI->ApplyCommand(command+fileName);
  
  delete runManager;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
