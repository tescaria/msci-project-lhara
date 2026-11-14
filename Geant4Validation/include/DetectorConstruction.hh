#ifndef DetectorConstruction_h
#define DetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "SimulationConfig.hh"
#include "globals.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

    DetectorConstruction(SimulationConfig& config);
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;

private:

    SimulationConfig& fConfig;
    G4GenericMessenger fMessenger;
};

#endif