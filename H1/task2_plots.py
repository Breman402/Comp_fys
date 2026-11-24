
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

# Load the standard simulation results
data = pd.read_csv("build/H1_task2_results.csv")

time = data["Time_ps"]
E_pot = data["E_pot"]
E_kin = data["E_kin"]
E_total = data["E_total"]
temperature = data["Temperature"]

# Detect the timestep from the CSV (in ps)
timestep = np.median(np.diff(time))
print(f"Timestep: {timestep:.6f} ps")

# Create a directory named after the timestep inside the H1 directory
timestep_dir = f"{timestep:.6f}ps"
output_dir = Path(__file__).resolve().parent / timestep_dir
output_dir.mkdir(parents=True, exist_ok=True)

# --------------------------------------------------
# Figure 1: Energy evolution
# --------------------------------------------------

plt.figure(figsize=(10, 5))

plt.plot(time, E_pot, label="Potential energy")
plt.plot(time, E_kin, label="Kinetic energy")
plt.plot(time, E_total, label="Total energy")

plt.xlabel("Time (ps)")
plt.ylabel("Energy (eV)")
plt.title(f"Energy evolution (timestep = {timestep:.6f} ps)")
plt.legend()
plt.grid(alpha=0.3)
plt.tight_layout()

plt.savefig(output_dir / "Task2_energies.png", dpi=300)
plt.show()

# --------------------------------------------------
# Figure 2: Temperature evolution
# --------------------------------------------------

# Exclude an optional initial t=0 row
temperature_samples = temperature[time > 0]
average_temperature = temperature_samples.mean()

print(f"Average temperature: {average_temperature:.2f} K")

plt.figure(figsize=(10, 5))

plt.plot(time, temperature, label="Instantaneous temperature")
plt.axhline(
    average_temperature,
    color="red",
    linestyle="--",
    label=f"Average = {average_temperature:.1f} K"
)

plt.xlabel("Time (ps)")
plt.ylabel("Temperature (K)")
plt.title(f"Temperature evolution (timestep = {timestep:.6f} ps)")
plt.legend()
plt.grid(alpha=0.3)
plt.tight_layout()

plt.savefig(output_dir / "Task2_temperature.png", dpi=300)
plt.show()

# --------------------------------------------------
# Figure 3: Energy conservation
# --------------------------------------------------

# Relative energy change compared with the first
# recorded total energy
E_initial = E_total.iloc[0]

relative_energy_change = (
    (E_total - E_initial) / abs(E_initial)
) * 100

plt.figure(figsize=(10, 5))

plt.plot(time, relative_energy_change)

plt.axhline(0, color="black", linestyle="--", alpha=0.5)

plt.xlabel("Time (ps)")
plt.ylabel("Relative energy change (%)")
plt.title(f"Conservation of total energy (timestep = {timestep:.6f} ps)")
plt.grid(alpha=0.3)
plt.tight_layout()

plt.savefig(output_dir / "Task2_energy_conservation.png", dpi=300)
plt.show()

print(f"All figures saved to: {output_dir.resolve()}")
