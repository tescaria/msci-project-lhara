#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4PhysListFactory.hh"

#include "SimulationConfig.hh"
#include "DetectorConstruction.hh"
#include "PhaseSpaceGeneratorAction.hh"
#include "DepthDoseScorer.hh"
#include "PhysicsConfiguration.hh"


int main(int argc, char** argv)
{
    // create shared simulation configuration
    SimulationConfig config;

    // create run manager
    auto runManager = new G4RunManager();

    // detector geometry and configuration messenger
    runManager->SetUserInitialization(new DetectorConstruction(config));

    // create reference physics list
    G4PhysListFactory factory;
    auto physics = factory.GetReferencePhysList("QGSP_BIC_HP");
    // register physics list 
    runManager->SetUserInitialization(physics);

    // physics configuration messenger
    PhysicsConfiguration physicsConfig(config, physics);

    // primary particle generator
    runManager->SetUserAction(new PhaseSpaceGeneratorAction(config));

    // scorer
    auto scorer = new DepthDoseScorer(config);
    runManager->SetUserAction(scorer);

    // UI manager
    auto ui = G4UImanager::GetUIpointer();

    // execute macro configuration commands
    if (argc > 1) {
        G4String command = "/control/execute ";
        G4String macro = argv[1];
        ui->ApplyCommand(command + macro);
    }
    else {
        runManager->Initialize();
    }

    scorer->WriteOutput();

    delete runManager;

    return 0;
}