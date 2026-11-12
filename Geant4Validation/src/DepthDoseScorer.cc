#include "DepthDoseScorer.hh"

#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4VTouchable.hh"

#include <fstream>
#include <iostream>
#include <iomanip>

// constructor: initialises all 400 array elements to zero
DepthDoseScorer::DepthDoseScorer()
{
    energyDeposit.fill(0.0);
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

    if (bin < 0 || bin >= nBins) {
        return;
    }

    // Apply the pre-step statistical weight
    G4double weight = preStep->GetWeight();

    energyDeposit[bin] += edep * weight;

}

void DepthDoseScorer::WriteOutput() const
{
    std::ofstream output("depth_dose.csv");

    if (!output.is_open()) {
        G4cerr << "Could not create depth_dose.csv" << G4endl;
        return;
    }

    auto water = G4NistManager::Instance()
                     ->FindOrBuildMaterial("G4_WATER");

    G4double density = water->GetDensity();

    // Volume and mass of each depth bin
    G4double sliceVolume =
        8.0 * cm * 6.0 * cm * 0.01 * mm;

    G4double sliceMass = density * sliceVolume;

    output << "Depth_mm,EnergyDeposit_MeV,Dose_Gy\n";
    output << std::scientific << std::setprecision(8);

    for (G4int i = 0; i < nBins; ++i) {

        G4double depth = (i + 0.5) * 0.01 * mm;

        G4double edepMeV = energyDeposit[i] / MeV;

        // dividing by sliceMass gives dose in MeV/g, dividing by gray converts to Gy
        G4double doseGy =
            (energyDeposit[i] / sliceMass) / gray;

        output << depth / mm << ","
               << edepMeV << ","
               << doseGy << "\n";
    }

    output.close();

    G4cout << "Depth-dose data written to depth_dose.csv"
           << G4endl;
}