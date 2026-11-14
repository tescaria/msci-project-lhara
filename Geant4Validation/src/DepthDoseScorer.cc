#include "DepthDoseScorer.hh"

#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4VTouchable.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Box.hh"

#include <fstream>
#include <iostream>
#include <iomanip>

// constructor: initialises all nBins array elements to zero
DepthDoseScorer::DepthDoseScorer(SimulationConfig& config)
    : fConfig(config),
      fMessenger(this, "/scoring/", "Scoring configuration")
{
    energyDeposit.assign(fConfig.nBins, 0.0);

    // define output file through macro
    fMessenger.DeclareProperty("outputFile", fConfig.outputFile, "Output dose filename");
}

void DepthDoseScorer::UserSteppingAction(const G4Step* step)
{
    // G4Step - particle current step
    // retrieve information about the particle immediately before the step
    auto preStep = step->GetPreStepPoint();
    // retrieve the geometry navigation info associated with that point
    auto touchable = preStep->GetTouchable();

    // only score energy deposited in the Z replicas
    auto volume = touchable->GetVolume();

    if (!volume || volume->GetName() != "ZDivision") {
        return;
    }

    // retrieve deposited energy during that step
    G4double edep = step->GetTotalEnergyDeposit();

    if (edep <= 0.0) {
        return;
    }

    // Get the depth bin from the Z replica copy number
    G4int bin = touchable->GetReplicaNumber(0);

    if (bin < 0 || bin >= fConfig.nBins) {
        return;
    }

    // Apply the pre-step statistical weight
    G4double weight = preStep->GetWeight();

    energyDeposit[bin] += edep * weight;

}

void DepthDoseScorer::WriteOutput() const
{
    std::ofstream output(fConfig.outputFile);

    if (!output.is_open()) {
        G4cerr << "Could not create " << fConfig.outputFile << G4endl;
        return;
    }

    // Retrieve the ZDivision logical volume
    auto logicalVolumes =
        G4LogicalVolumeStore::GetInstance();

    auto logicZ = logicalVolumes->GetVolume("ZDivision");

    if (!logicZ) {
        G4cerr << "Could not find ZDivision logical volume"
               << G4endl;
        return;
    }

    // Retrieve the actual geometry and material
    auto solid = dynamic_cast<G4Box*>(logicZ->GetSolid());

    if (!solid) {
        G4cerr << "ZDivision is not a G4Box" << G4endl;
        return;
    }

    auto material = logicZ->GetMaterial();

    G4double density = material->GetDensity();

    // Calculate bin dimensions
    G4double binWidth = 2.0 * solid->GetZHalfLength();

    G4double sliceVolume =
        (2.0 * solid->GetXHalfLength()) *
        (2.0 * solid->GetYHalfLength()) *
        binWidth;

    G4double sliceMass = density * sliceVolume;

    output << "Depth_mm,EnergyDeposit_MeV,Dose_Gy\n";
    output << std::scientific << std::setprecision(8);

    for (G4int i = 0; i < fConfig.nBins; ++i) {

        G4double depth = (i + 0.5) * binWidth;

        G4double edepMeV = energyDeposit[i] / MeV;

        G4double doseGy =
            (energyDeposit[i] / sliceMass) / gray;

        output << depth / mm << ","
               << edepMeV << ","
               << doseGy << "\n";
    }

    output.close();

    G4cout << "Depth-dose data written to "
       << fConfig.outputFile << G4endl;;
}