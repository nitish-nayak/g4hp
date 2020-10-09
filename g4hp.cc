
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
#include "HPConfig.hh"

#include <iostream>
#include <fstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv) {
  HPConfig config (argc, argv);
  
  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine());
  
  //Construct the default run manager
  G4RunManager * runManager = new G4RunManager();
  
  //set mandatory initialization classes
  runManager->SetUserInitialization(new HPDetectorConstruction(config.getTarget()));
  
  if(config.getphysicsList()=="QGSP_BERT"){  
    QGSP_BERT* qgsp_bert_List = new QGSP_BERT;
    std::cout<<"=> Actual physics list used in the run manager: "<<
      config.getphysicsList()<<std::endl;
    runManager->SetUserInitialization(qgsp_bert_List);    
  }
  else if(config.getphysicsList()=="FTFP_BERT"){  
    FTFP_BERT* ftfp_bert_List = new FTFP_BERT;
    std::cout<<"=> Actual physics list used in the run manager: "<<
      config.getphysicsList()<<std::endl;
    runManager->SetUserInitialization(ftfp_bert_List);    
  }
  else{
    std::cout<<"=> This code currently only support QGSP_BERT and FTFP_BERT"<<std::endl;
    exit (1);
  }
  
  runManager->SetUserAction(new HPPrimaryGeneratorAction());
  
  //set user action classes
  runManager->SetUserAction(new HPRunAction(config));
  runManager->SetUserAction(new HPEventAction());
  runManager->SetUserAction(new HPStackingAction());
  runManager->SetUserAction(new HPTrackingAction());
  
  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();
  
  UI->ApplyCommand("/control/verbose 0");
  UI->ApplyCommand("/run/verbose 0");
  UI->ApplyCommand("/tracking/verbose 0");
  UI->ApplyCommand("/testhadr/Update");
  UI->ApplyCommand("/run/initialize");
  
  UI->ApplyCommand("/random/setSeeds " + config.getBeam().energy + " "
                   + config.getRunNumber());
  UI->ApplyCommand("/gun/particle " + config.getBeam().particle);
  UI->ApplyCommand("/gun/energy " + config.getBeam().energy + " GeV");
  UI->ApplyCommand("/run/beamOn " + config.getNevents());
  
  UI->ApplyCommand("/control/execute");
  
  delete runManager;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
