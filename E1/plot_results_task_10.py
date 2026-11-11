import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("./build/time_averages.csv")

plt.figure(figsize=(12, 6))

for k in range(1, 33):
    plt.plot(
        df["time"],
        df[f"E{k}"],
        label=f"Mode {k}"
    )

plt.xscale("log")
plt.yscale("log")

plt.xlabel("Time")
plt.ylabel("Cumulative time-averaged energy")
plt.title("FPUT - Time-averaged normal mode energies")

plt.grid(True, which="both", alpha=0.3)
plt.legend(bbox_to_anchor=(1.02, 1), loc="upper left", ncol=2)

plt.tight_layout()
plt.show()