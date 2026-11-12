import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("task1_results.csv")

a = data["Lattice_Param"].to_numpy()
E = data["E_pot"].to_numpy()

# Convert lattice parameter to unit cell volume
V = a**3

# Fit quadratic polynomial to energy vs volume
A, B, C = np.polyfit(V, E, 2)

# Find equilibrium volume and lattice parameter
V0 = -B / (2 * A)
a0 = np.cbrt(V0)

print(f"Theoretical lattice parameter at 0 K: {a0:.5f} Å")
print(f"Equilibrium unit cell volume: {V0:.5f} Å³")

# Generate fitted curve
V_fit = np.linspace(min(V), max(V), 200)
E_fit = np.polyval([A, B, C], V_fit)

plt.scatter(V, E, label="Simulation data")
plt.plot(V_fit, E_fit, "--", label="Quadratic fit")

plt.xlabel("Unit cell volume (Å³)")
plt.ylabel("Energy (eV/unit cell)")
plt.legend()
plt.grid()
plt.show()