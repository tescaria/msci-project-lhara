import numpy as np


def analyse_phase_space(filename):
    data = np.loadtxt(filename)

    x = data[:, 0]  # cm
    y = data[:, 1]  # cm
    energy = data[:, 5]  # MeV
    weight = data[:, 6]

    n_particles = len(energy)
    total_weight = np.sum(weight)
    mean_energy = np.average(energy, weights=weight)
    total_energy = np.sum(energy * weight)

    print(f"\n{filename}")
    print(f"Particles: {n_particles}")
    print(f"Total weight: {total_weight}")
    print(f"Mean energy: {mean_energy:.4f} MeV")
    print(f"Minimum energy: {energy.min():.4f} MeV")
    print(f"Maximum energy: {energy.max():.4f} MeV")
    print(f"Total incident energy: {total_energy:.4f} MeV")

    # Transverse position analysis
    print("\n--- Transverse position analysis ---")

    print(f"X minimum: {x.min():.6f} cm")
    print(f"X maximum: {x.max():.6f} cm")
    print(f"Maximum |X|: {np.max(np.abs(x)):.6f} cm")

    print(f"Y minimum: {y.min():.6f} cm")
    print(f"Y maximum: {y.max():.6f} cm")
    print(f"Maximum |Y|: {np.max(np.abs(y)):.6f} cm")

    # Radial distance from the beam origin
    r = np.sqrt(x**2 + y**2)

    print(f"Maximum radial distance: {r.max():.6f} cm")

    return total_energy


files = [
    "PoPLaR-gaussian_4MeV.phsp",
    "PoPLaR-gaussian_8MeV.phsp",
    "PoPLaR-gaussian_16MeV.phsp"
]

for filename in files:
    analyse_phase_space(filename)