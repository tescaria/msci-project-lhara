#include "G4RunManagerFactory.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "G4PhysListFactory.hh"

#include "DetectorConstruction.hh"
#include "PhaseSpaceGeneratorAction.hh"
#include "DepthDoseScorer.hh"

int main(int argc, char** argv)
{
    // Create run manager
    auto runManager = new G4RunManager();

    // Detector geometry
    runManager->SetUserInitialization(new DetectorConstruction());

    // Physics list
    G4PhysListFactory factory;
    auto physics = factory.GetReferencePhysList("QGSP_BIC_HP");
    runManager->SetUserInitialization(physics);

    // Primary particle generator
    runManager->SetUserAction(new PhaseSpaceGeneratorAction());

    auto scorer = new DepthDoseScorer();
    runManager->SetUserAction(scorer);

    // Initialise simulation
    runManager->Initialize();

    // UI manager
    auto ui = G4UImanager::GetUIpointer();

    // execute macro file if provided as command line argument
    if (argc > 1) {
        G4String command = "/control/execute ";
        G4String macro = argv[1];
        ui->ApplyCommand(command + macro);
    }

    scorer->WriteOutput();

    delete runManager;

    return 0;
}