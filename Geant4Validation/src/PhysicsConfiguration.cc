#include "PhysicsConfiguration.hh"

#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option6.hh"
#include "G4Exception.hh"
#include "G4DNAGenericIonsManager.hh"

PhysicsConfiguration::PhysicsConfiguration(
    SimulationConfig& config,
    G4VModularPhysicsList* physics
)
    : fConfig(config),
      fPhysics(physics),
      fMessenger(this, "/physics/", "Physics configuration")
{
    fMessenger.DeclareMethod(
        "emPhysics",
        &PhysicsConfiguration::SetEMPhysics,
        "Electromagnetic physics configuration"
    );
}

void PhysicsConfiguration::SetEMPhysics(const G4String& physics)
{
    fConfig.emPhysics = physics;

    if (physics == "DNA_option2") {
        G4DNAGenericIonsManager::Instance();
        fPhysics->ReplacePhysics(new G4EmDNAPhysics_option2());
    }
    else if (physics == "DNA_option4") {
        G4DNAGenericIonsManager::Instance();
        fPhysics->ReplacePhysics(new G4EmDNAPhysics_option4());
    }
    else if (physics == "DNA_option6") {
        G4DNAGenericIonsManager::Instance();
        fPhysics->ReplacePhysics(new G4EmDNAPhysics_option6());
    }
    else if (physics != "default") {
        G4Exception(
            "PhysicsConfiguration::SetEMPhysics",
            "InvalidPhysics",
            FatalException,
            "Unknown electromagnetic physics configuration."
        );
    }
}