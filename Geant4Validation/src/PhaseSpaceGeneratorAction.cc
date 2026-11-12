#include "PhaseSpaceGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Proton.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

#include <fstream>
#include <cmath>
#include <stdexcept>

// constructor: initialise the generator
PhaseSpaceGeneratorAction::PhaseSpaceGeneratorAction()
{
    // create particle gun - one primary particle per call to GeneratePrimaryVertex()
    particleGun = new G4ParticleGun(1);

    // define particle type as proton
    particleGun->SetParticleDefinition(G4Proton::ProtonDefinition());

    // open phase-space file
    inputFile.open("phase_space/beam8MeV.phsp");

    if (!inputFile.is_open()) {
        throw std::runtime_error(
            "Could not open phase-space file"
        );
    }
}

// destructor: clean up
PhaseSpaceGeneratorAction::~PhaseSpaceGeneratorAction()
{
    // release memory allocated with new and close the phase-space file
    delete particleGun;
    inputFile.close();
}

// function called at the beginning of each event to generate the primary particle(s)
void PhaseSpaceGeneratorAction::GeneratePrimaries(
    G4Event* event)
{
    G4double x, y, z;
    G4double u, v, energy;
    G4double weight;
    G4int pdg, negativeW, firstParticle;

    // read the next particle from the phase-space file
    if (!(inputFile >> x >> y >> z
                    >> u >> v >> energy
                    >> weight >> pdg
                    >> negativeW >> firstParticle)) {

        G4Exception(
            "PhaseSpaceGeneratorAction",
            "001",
            FatalException,
            "Could not read next phase-space particle"
        );
    }

    // reconstruc the longitudinal direction cosine
    G4double w = std::sqrt(1.0 - u*u - v*v);

    if (negativeW == 1) {
        w = -w;
    }

    // convert position into Geant4 vector
    // *cm converts from cm to Geant4 internal units (mm)
    G4ThreeVector position(
        x * cm,
        y * cm,
        z * cm
    );

    // construct the direction vector and normalise it to unit length
    G4ThreeVector direction(u, v, w);
    direction = direction.unit();
    // set the particle properties
    particleGun->SetParticlePosition(position);
    particleGun->SetParticleMomentumDirection(direction);
    particleGun->SetParticleEnergy(energy * MeV);
    particleGun->SetParticleWeight(weight);
    // generate primary vertex (starting point of a primary particle in an event)
    particleGun->GeneratePrimaryVertex(event);
}