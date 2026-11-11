
from pathlib import Path
import pandas as pd
import matplotlib.pyplot as plt
import plotly.graph_objects as go
import webbrowser

# =========================
# User settings
# =========================
csv_file = "./build/energy_records.csv"
output_dir = Path("E1/Task8")
output_dir.mkdir(parents=True, exist_ok=True)

# =========================
# Read simulation data
# =========================
df = pd.read_csv(csv_file)

# Calculate timestep
timestep = df["time"].diff().dropna().median()
timestep_str = f"{timestep:.10g}"

print(f"Detected timestep: {timestep_str}")

# =========================
# Plot 1: Static Matplotlib plot
# =========================
fig, ax = plt.subplots(figsize=(10, 5))

for k in range(1, 6):
    ax.plot(
        df["time"],
        df[f"E{k}"],
        label=f"Mode {k}"
    )

ax.set_xlabel("Time")
ax.set_ylabel("Normal mode energy")
ax.set_title("FPUT Harmonic System - Normal Mode Energies")
ax.legend(loc="upper right")
ax.grid(True, alpha=0.3)

ax.text(
    0.02, 0.95,
    f"Δt = {timestep_str}, α = 0",
    transform=ax.transAxes,
    verticalalignment="top",
    bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
)

fig.tight_layout()
fig.savefig(output_dir / "normal_mode_energies.png", dpi=300)
plt.close(fig)

# =========================
# Plot 2: Interactive Plotly plot
# =========================
fig_interactive = go.Figure()

for k in range(1, 6):
    fig_interactive.add_trace(
        go.Scattergl(
            x=df["time"],
            y=df[f"E{k}"],
            mode="lines",
            name=f"Mode {k}"
        )
    )

fig_interactive.update_layout(
    title="FPUT Harmonic System - Normal Mode Energies",
    xaxis_title="Time",
    yaxis_title="Normal mode energy",
    template="plotly_white",
    hovermode="x unified",
    width=1100,
    height=550
)

fig_interactive.add_annotation(
    x=0.02,
    y=0.95,
    xref="paper",
    yref="paper",
    text=f"Δt = {timestep_str}, α = 0",
    showarrow=False,
    bgcolor="white",
    bordercolor="black",
    borderwidth=1
)

html_path = output_dir / "normal_mode_energies.html"

fig_interactive.write_html(
    html_path,
    include_plotlyjs=True
)

# Open interactive plot
webbrowser.open(html_path.resolve().as_uri())

# =========================
# Verify energy conservation
# =========================
print("\nNormal mode energy statistics:")

for k in range(1, 6):
    energy = df[f"E{k}"]

    print(
        f"Mode {k}: "
        f"Initial = {energy.iloc[0]:.8g}, "
        f"Final = {energy.iloc[-1]:.8g}, "
        f"Min = {energy.min():.8g}, "
        f"Max = {energy.max():.8g}"
    )

print(f"\nPlots saved in: {output_dir.resolve()}")
