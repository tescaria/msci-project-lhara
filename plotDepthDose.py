import numpy as np 
import matplotlib.pyplot as plt
import seaborn as sns
sns.set_style("darkgrid")


def plot_multiple_energy_dose(data_paths, labels, output_path):
    """
    Plot multiple depth-dose curves on the same graph, for different incident energies.
    Files expected to only contain one bin in x and y directions.
    """

    thickness = 4
    nbins = 400
    bin_width = thickness / nbins

    fig, ax = plt.subplots()

    for data_path, label in zip(data_paths, labels):
        data = np.loadtxt(data_path, delimiter=",", skiprows=8, unpack=True)
        _, _, zbins, dose = data

        depth = (zbins + 0.5) * bin_width

        if 1==1:
            peak_index = np.argmax(dose)
            peak_position = depth[peak_index]
            peak_dose = dose[peak_index]
            print(f"{label}: Bragg peak at {peak_position:.3f} mm "
                f"(Dose: {peak_dose:.6f} Gy)")

        ax.plot(depth, dose, label=label)

    ax.set_xlabel("Depth [mm]")
    ax.set_ylabel("Dose [Gy]")
    ax.legend()
    plt.savefig(output_path)


def plot_multiple_energy_deposit(data_paths, labels, output_path):
    """
    Plot multiple depth-energy deposition curves on the same graph, for different incident energies.
    Files expected to only contain one bin in x and y directions.
    """

    thickness = 4
    nbins = 400
    bin_width = thickness / nbins

    fig, ax = plt.subplots()

    for data_path, label in zip(data_paths, labels):
        data = np.loadtxt(data_path, delimiter=",", skiprows=8, unpack=True)
        _, _, zbins, energy_deposit = data

        depth = (zbins + 0.5) * bin_width

        ax.plot(depth, energy_deposit, label=label)

    ax.set_xlabel("Depth [mm]")
    ax.set_ylabel("Energy deposited [MeV]")
    ax.legend()
    plt.savefig(output_path)


def plot_dose_xy(data_path, depth_mm, output_path):
    """Plots a 2d xy dose heatmap at one selected depth"""
    
    data = np.loadtxt(data_path, delimiter=",", skiprows=8)

    xbins = data[:, 0].astype(int)
    ybins = data[:, 1].astype(int)
    zbins = data[:, 2].astype(int)
    dose = data[:, 3]
    nx, ny, nz = 80, 60, 400

    dose_3d = np.zeros((nz, ny, nx))
    dose_3d[zbins, ybins, xbins] = dose

    # Select the closest Z bin to the requested depth
    z_index = int(depth_mm / (4 / nz))
    z_index = min(z_index, nz - 1)

    dose_xy = dose_3d[z_index, :, :]

    fig, ax = plt.subplots()

    image = ax.imshow(
        dose_xy,
        origin="lower",
        extent=[-4, 4, -3, 3],
        aspect="equal",
        cmap="inferno"
    )

    ax.set_xlabel("X [cm]")
    ax.set_ylabel("Y [cm]")
    ax.set_title(f"Dose distribution at z = {(z_index + 0.5) * 4/nz:.3f} mm")
    #ax.set_xlim(-1, 1)
    #ax.set_ylim(-1, 1)

    fig.colorbar(image, ax=ax, label="Dose [Gy]", format="%.2e")

    plt.savefig(output_path)


def print_integrated_energy_deposit(data_paths, labels, total_incident_energy):

    integrated_energies = []

    for data_path, label in zip(data_paths, labels):
        data = np.loadtxt(data_path, delimiter=",", skiprows=8, unpack=True)
        _, _, zbins, energy_deposit = data

        integrated_energy = np.sum(energy_deposit)
        integrated_energies.append(integrated_energy)

        print(f"\n{label}")
        print(f"Total deposited energy: {integrated_energy:.6f} MeV")
        print(f"Incident energy: {total_incident_energy[len(integrated_energies)-1]:.6f} MeV")
        print(f"Deposited fraction: {integrated_energy / total_incident_energy[len(integrated_energies)-1]:.4%}")

    print("\n--- Energy comparison ---")

    for i in range(1, len(labels)):
        deposited_ratio = integrated_energies[i] / integrated_energies[i-1]
        incident_ratio = total_incident_energy[i] / total_incident_energy[i-1]

        print(f"\n{labels[i]} / {labels[i-1]}")
        print(f"Deposited energy ratio: {deposited_ratio:.4f}")
        print(f"Incident energy ratio: {incident_ratio:.4f}")

def plot_depth_dose_g4(data_path, output_path):
    """
    Plot depth-dose curve from Geant4 simulation output.
    Files contain depth in mm, energy deposition in MeV, and dose in Gy.
    """

    fig, ax = plt.subplots()

    data = np.loadtxt(data_path, delimiter=",", skiprows=1, unpack=True)
    depth, energy, dose = data

    ax.plot(depth, dose, label="Geant4")

    ax.set_xlabel("Depth [mm]")
    ax.set_ylabel("Dose [Gy]")
    ax.legend()
    plt.savefig(output_path)

def plot_dose_topas_g4(topas_data_path, g4_data_path, output_path):
    """
    Plot depth-dose curves from both TOPAS and Geant4 simulation outputs.
    """

    fig, ax = plt.subplots()

    topas_data = np.loadtxt(topas_data_path, delimiter=",", skiprows=8, unpack=True)
    _, _, zbins, topas_dose = topas_data
    thickness = 4
    nbins = 400
    bin_width = thickness / nbins
    topas_depth = (zbins + 0.5) * bin_width

    g4_data = np.loadtxt(g4_data_path, delimiter=",", skiprows=1, unpack=True)
    g4_depth, g4_energy, g4_dose = g4_data

    ax.plot(topas_depth, topas_dose, 'o', color='black', ms=3,label="TOPAS")
    ax.plot(g4_depth, g4_dose, 'x', color='orange', ms=5, label="Geant4")

    ax.set_xlabel("Depth [mm]")
    ax.set_ylabel("Dose [Gy]")
    ax.legend()
    plt.savefig(output_path)

def main():

    labels = ["4 MeV", "8 MeV", "16 MeV"]
    
    # dose plot
    data_paths = [
       "dose_energy_csv/topas/DepthDose4MeV.csv",
       "dose_energy_csv/topas/DepthDose8MeV.csv",
       "dose_energy_csv/topas/DepthDose16MeV.csv"
       ]
    output_path = "DepthDoseComparison.png"
    #plot_multiple_energy_dose(data_paths, labels, output_path)

    # energy deposition plots
    energy_data_paths = [
       "dose_energy_csv/topas/EnergyDeposit4MeV.csv",
       "dose_energy_csv/topas/EnergyDeposit8MeV.csv",
       "dose_energy_csv/topas/EnergyDeposit16MeV.csv"
       ]
    energy_output_path = "EnergyDepositComparison.png"
    #plot_multiple_energy_deposit(energy_data_paths, labels, energy_output_path)

    #plot_dose_xy("dose_energy_csv_xyz/DepthDose4MeV.csv", depth_mm=0.215, output_path="DoseXY4MeV_0.215mm.png")
    #plot_depth_dose_g4("dose_energy_csv/geant4Validation/depth_dose.csv", output_path="plots/DepthDoseG4.png")
    plot_dose_topas_g4("dose_energy_csv/topas/DepthDose8MeV.csv", "dose_energy_csv/geant4Validation/dose_8MeV.csv", output_path="plots/DepthDoseTOPAS_G4_v2.png")


if __name__ == "__main__":
    main()