# Geant4Validation

Geant4 simulation framework for validating particle transport and energy deposition. The code is currently being extended to support cellular phantom simulations.

## Code structure

| Component | Description |
|---|---|
| `SimulationConfig.hh` | Stores all shared simulation parameters. |
| `DetectorConstruction` | Builds the simulation geometry from the configuration. |
| `PhaseSpaceGeneratorAction` | Generates particles from the selected input phase-space file (stored in `phase_space/` and produced by `dat_to_ascii.py`). |
| `DepthDoseScorer` | Scores energy and dose deposition using the configured binning. |
| `PhysicsConfiguration` | Handles electromagnetic physics configuration. |
| `main.cc` | Creates and connects all simulation components. |
| `.mac` files | Define individual simulation configurations and runtime parameters. |

## Running a simulation

From this directory:

```bash
mkdir build
cmake -S . -B build
cmake --build build -j$(nproc)
./build/Geant4Validation macros/run.mac
```
Replace `macros/run.mac` with the desired macro file to run a different configuration.
