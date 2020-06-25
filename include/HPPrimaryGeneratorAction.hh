
#ifndef HPPrimaryGeneratorAction_h
#define HPPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class HPPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  HPPrimaryGeneratorAction();
  virtual ~HPPrimaryGeneratorAction();

  void GeneratePrimaries(G4Event*);
  G4ParticleGun* GetParticleGun() {return particleGun;};

private:

  HPPrimaryGeneratorAction & operator=(const HPPrimaryGeneratorAction &right);
  HPPrimaryGeneratorAction(const HPPrimaryGeneratorAction&);

  G4ParticleGun*   particleGun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


