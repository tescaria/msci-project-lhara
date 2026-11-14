#ifndef DepthDoseScorer_h
#define DepthDoseScorer_h

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"
#include "SimulationConfig.hh"
#include "G4GenericMessenger.hh"

#include <vector>

class G4Step;

class DepthDoseScorer : public G4UserSteppingAction
{
public:
    DepthDoseScorer(SimulationConfig& config);
    ~DepthDoseScorer() override = default;

    void UserSteppingAction(const G4Step* step) override;

    void WriteOutput() const;

private:

    SimulationConfig& fConfig;
    G4GenericMessenger fMessenger;
    std::vector<G4double> energyDeposit;
};

#endif