#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4PhysListFactory.hh"

#include "SimulationConfig.hh"
#include "DetectorConstruction.hh"
#include "PhaseSpaceGeneratorAction.hh"
#include "DepthDoseScorer.hh"

int main(int argc, char** argv)
{
    // Create shared simulation configuration
    SimulationConfig config;

    // Create run manager
    auto runManager = new G4RunManager();

    // Detector geometry
    runManager->SetUserInitialization(new DetectorConstruction(config));

    // Physics list
    G4PhysListFactory factory;
    auto physics = factory.GetReferencePhysList("QGSP_BIC_HP");
    runManager->SetUserInitialization(physics);

    // Primary particle generator
    runManager->SetUserAction(new PhaseSpaceGeneratorAction(config));

    auto scorer = new DepthDoseScorer(config);
    runManager->SetUserAction(scorer);

    // UI manager
    auto ui = G4UImanager::GetUIpointer();

    // Execute macro file if provided as command line argument
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