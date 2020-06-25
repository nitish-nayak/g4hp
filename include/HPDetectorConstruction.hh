
#ifndef HPDetectorConstruction_h
#define HPDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"

class G4LogicalVolume;
class HPDetectorMessenger;

class HPDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  HPDetectorConstruction();
  virtual ~HPDetectorConstruction();

public:

  G4VPhysicalVolume* Construct();

  void SetTargetMaterial(const G4String&);

  void UpdateGeometry();

private:

  HPDetectorConstruction & operator=(const HPDetectorConstruction &right);
  HPDetectorConstruction(const HPDetectorConstruction&);

  G4double radius;

  G4double  TargetZ, TargetA, TargetDensity; 

  G4Material*  targetMaterial;
  G4Material*  worldMaterial;

  G4LogicalVolume* logicTarget;
  G4LogicalVolume* logicWorld;

  HPDetectorMessenger* detectorMessenger;

};

#endif

