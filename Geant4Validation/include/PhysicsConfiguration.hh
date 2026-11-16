#ifndef PhysicsConfiguration_h
#define PhysicsConfiguration_h

#include "G4GenericMessenger.hh"
#include "G4VModularPhysicsList.hh"
#include "SimulationConfig.hh"

class PhysicsConfiguration
{
public:

    PhysicsConfiguration(
        SimulationConfig& config,
        G4VModularPhysicsList* physics
    );

    ~PhysicsConfiguration() = default;

    void SetEMPhysics(const G4String& physics);

private:

    SimulationConfig& fConfig;
    G4VModularPhysicsList* fPhysics;
    G4GenericMessenger fMessenger;
};

#endif