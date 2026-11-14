#ifndef SimulationConfig_h
#define SimulationConfig_h

#include "globals.hh"
#include "G4SystemOfUnits.hh"

struct SimulationConfig
{
    // set default values
    // Phantom configuration
    G4double sizeX = 8.0 * cm;
    G4double sizeY = 6.0 * cm;
    G4double sizeZ = 4.0 * mm;

    G4String material = "G4_WATER";

    // Scoring configuration
    G4int nBins = 400;

    // Source configuration
    G4String phaseSpaceFile = "phase_space/beam8MeV.phsp";

    // Output configuration
    G4String outputFile = "depth_dose.csv";
};

#endif