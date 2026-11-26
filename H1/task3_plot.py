import pandas as pd
import matplotlib.pyplot as plt

eq = pd.read_csv("H1_task3_prod.csv")

fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))

ax[0].plot(eq["time"], eq["temperature"])
ax[0].axhline(773.15, color="red", linestyle="--")
ax[0].set_ylabel("Temperature [K]")

ax[1].plot(eq["time"], eq["pressure_bar"])
ax[1].axhline(1.0, color="red", linestyle="--")
ax[1].set_ylabel("Pressure [bar]")

ax[2].plot(eq["time"], eq["lattice_parameter"])
ax[2].set_ylabel("Lattice parameter [Å]")
ax[2].set_xlabel("Time [ps]")

plt.tight_layout()
plt.show()