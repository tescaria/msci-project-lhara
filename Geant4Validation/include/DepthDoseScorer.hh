#ifndef DepthDoseScorer_h
#define DepthDoseScorer_h

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"

#include <array>

class G4Step;

class DepthDoseScorer : public G4UserSteppingAction
{
public:
    DepthDoseScorer();
    ~DepthDoseScorer() override = default;

    void UserSteppingAction(const G4Step* step) override;

    void WriteOutput() const;

private:
    static constexpr G4int nBins = 400;

    std::array<G4double, nBins> energyDeposit;
};

#endif