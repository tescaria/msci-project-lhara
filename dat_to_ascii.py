import argparse
import math
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "01-Code"))

import BeamIO
import Particle as Prtcl


TARGET = "PoPLaR:1:Capture:Drift:3"
PROTON_MASS_MEV = 938.2720813
PROTON_PDG = 2212


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", default="dat_ascii_files/PoPLaR-gaussian_8MeV.dat")
    parser.add_argument("-o", "--output", default="dat_ascii_files/PoPLaR-gaussian_8MeV.phsp")
    parser.add_argument("--max-records", type=int, default=None, help="Optional limit for testing")
    args = parser.parse_args()

    input_path = Path(args.input).resolve()
    output_path = Path(args.output).resolve()
    header_path = output_path.with_suffix(".header")

    if not input_path.is_file():
        raise FileNotFoundError(input_path)

    reader = BeamIO.BeamIO(None, str(input_path))
    n_records = 0

    while True:
        eof = reader.readBeamDataRecord()
        if eof:
            break

        n_records += 1
        if args.max_records is not None and n_records >= args.max_records:
            break

    particles = [
        p for p in Prtcl.Particle.getinstances()
        if not isinstance(p, Prtcl.ReferenceParticle)
    ]

    print(f"Binary records read: {n_records}")
    print(f"Simulated particle instances: {len(particles)}")

    if not particles:
        raise RuntimeError("No simulated particles were read.")

    success = Prtcl.Particle.fillPhaseSpaceAll()
    print(f"Phase-space calculation success: {success}")

    # The reference particle defines the location and orientation
    # of the requested output plane.
    reference = next(
        p for p in Prtcl.Particle.getinstances()
        if isinstance(p, Prtcl.ReferenceParticle)
    )

    if TARGET not in reference.getLocation():
        raise RuntimeError(f"Target location not found: {TARGET}")

    ref_idx = reference.getLocation().index(TARGET)
    reference_lab = reference.getLabPhaseSpace()[ref_idx]
    # longitudinal position of the target scoring plane 
    # (is set as 0 in exported phase space file)
    z_plane_m = float(reference_lab[0][2])

    records = []
    skipped_no_plane = 0
    skipped_invalid = 0

    for particle in particles:
        locations = particle.getLocation()

        if TARGET not in locations:
            skipped_no_plane += 1
            continue

        idx = locations.index(TARGET)
        phase_space = particle.getLabPhaseSpace()[idx]

        if phase_space is None or len(phase_space) != 2:
            skipped_invalid += 1
            continue

        position = np.asarray(phase_space[0], dtype=float)
        momentum = np.asarray(phase_space[1], dtype=object)

        if position.size != 3 or momentum.size != 3:
            skipped_invalid += 1
            continue

        if any(v is None for v in momentum):
            skipped_invalid += 1
            continue

        momentum = np.asarray(momentum, dtype=float)

        if not np.all(np.isfinite(position)) or not np.all(
            np.isfinite(momentum)
        ):
            skipped_invalid += 1
            continue

        px, py, pz = momentum
        p_mag = float(np.linalg.norm(momentum))

        if p_mag <= 0:
            skipped_invalid += 1
            continue

        kinetic_energy = (
            math.sqrt(p_mag**2 + PROTON_MASS_MEV**2) - PROTON_MASS_MEV)

        # OpenTOPAS positions are in cm. Set Z=0 at the
        # selected phase-space plane.
        x_cm = position[0] * 100.0
        y_cm = position[1] * 100.0
        z_cm = (position[2] - z_plane_m) * 100.0

        u = px / p_mag
        v = py / p_mag
        negative_w = int(pz < 0)

        # Each LLO proton is treated as an independent primary
        # history, so each is the first scored particle of its history.
        first_particle = 1
        weight = 1.0

        records.append((
            x_cm, y_cm, z_cm, u, v, kinetic_energy,
            weight, PROTON_PDG, negative_w, first_particle
        ))

    if not records:
        raise RuntimeError(
            f"No valid particles reached {TARGET}. "
            "No output files written."
        )

    output_path.parent.mkdir(parents=True, exist_ok=True)

    with output_path.open("w") as f:
        for row in records:
            f.write(
                f"{row[0]: .9e} {row[1]: .9e} {row[2]: .9e} "
                f"{row[3]: .9e} {row[4]: .9e} {row[5]: .9e} "
                f"{row[6]:.1f} {row[7]:d} {row[8]:d} {row[9]:d}\n"
            )

    energies = [row[5] for row in records]
    n_original = len(particles)
    n_scored = len(records)

    with header_path.open("w") as f:
        f.write(
            "TOPAS ASCII Phase Space\n\n"
            f"Number of Original Histories: {n_original}\n"
            f"Number of Original Histories that Reached Phase Space: "
            f"{n_scored}\n"
            f"Number of Scored Particles: {n_scored}\n\n"
            "Columns of data are as follows:\n"
            " 1: Position X [cm]\n"
            " 2: Position Y [cm]\n"
            " 3: Position Z [cm]\n"
            " 4: Direction Cosine X\n"
            " 5: Direction Cosine Y\n"
            " 6: Energy [MeV]\n"
            " 7: Weight\n"
            " 8: Particle Type (in PDG Format)\n"
            " 9: Flag to tell if Third Direction Cosine is Negative "
            "(1 means true)\n"
            "10: Flag to tell if this is the First Scored Particle "
            "from this History (1 means true)\n\n"
            f"Number of proton: {n_scored}\n\n"
            f"Minimum Kinetic Energy of proton: {min(energies):.6f} MeV\n"
            f"Maximum Kinetic Energy of proton: {max(energies):.6f} MeV\n"
        )

    print(f"Output plane: {TARGET}")
    print(f"Reference plane Z: {z_plane_m:.6f} m")
    print(f"Exported protons: {n_scored}")
    print(f"Skipped (did not reach plane): {skipped_no_plane}")
    print(f"Skipped (invalid phase space): {skipped_invalid}")
    print(f"Phase-space file: {output_path}")
    print(f"Header file:      {header_path}")


if __name__ == "__main__":
    main()