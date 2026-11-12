#ifndef PhaseSpaceGeneratorAction_h
#define PhaseSpaceGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include <fstream>

class G4ParticleGun;
class G4Event;

class PhaseSpaceGeneratorAction
    : public G4VUserPrimaryGeneratorAction
{
public:

    PhaseSpaceGeneratorAction();
    ~PhaseSpaceGeneratorAction();

    void GeneratePrimaries(G4Event* event) override;

private:

    G4ParticleGun* particleGun;
    std::ifstream inputFile;
};

#endif