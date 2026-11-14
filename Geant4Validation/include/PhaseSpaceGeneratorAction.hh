#ifndef PhaseSpaceGeneratorAction_h
#define PhaseSpaceGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "SimulationConfig.hh"
#include "globals.hh"

#include <fstream>

class G4ParticleGun;
class G4Event;

class PhaseSpaceGeneratorAction
    : public G4VUserPrimaryGeneratorAction
{
public:

    PhaseSpaceGeneratorAction(SimulationConfig& config);
    ~PhaseSpaceGeneratorAction() override;

    void GeneratePrimaries(G4Event* event) override;

private:

    G4ParticleGun* particleGun;
    std::ifstream inputFile;

    SimulationConfig& fConfig;
    G4GenericMessenger fMessenger;
};

#endif