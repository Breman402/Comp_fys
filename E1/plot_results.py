
from pathlib import Path
import webbrowser

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import plotly.graph_objects as go

# =========================
# User settings
# =========================
time_unit = "ps"

simulation_csv = "./build/simulation_data.csv"
atom_csv = "./build/atom_data.csv"

# Automatically open interactive plots in browser
open_interactive_plots = True

# Limit plotted points for browser performance.
# Full data is still used for FFT and PNG plots.
max_interactive_points = 12000

# =========================
# Read simulation data
# =========================
sim_df = pd.read_csv(
    simulation_csv,
    header=None,
    names=["time", "potential", "kinetic", "total"]
)

if len(sim_df) < 2:
    raise ValueError("At least two measurements are required.")

time_differences = np.diff(sim_df["time"].to_numpy())
timestep = float(np.median(time_differences))

if timestep <= 0:
    raise ValueError("Invalid timestep detected.")

if not np.allclose(time_differences, timestep, rtol=1e-6):
    raise ValueError("Simulation timestamps are not evenly spaced.")

# Convert energy from eV to meV
sim_df[["potential", "kinetic", "total"]] *= 1000

timestep_str = f"{timestep:.10g}"

output_dir = Path("E1") / timestep_str
output_dir.mkdir(parents=True, exist_ok=True)

print(f"Detected timestep: {timestep_str} {time_unit}")

# =========================
# Read atom data
# =========================
atom_raw = pd.read_csv(
    atom_csv,
    header=None,
    names=["time", "displacement"]
)

if len(atom_raw) != 3 * len(sim_df):
    raise ValueError(
        "Expected three atom measurements per timestep."
    )

atom1 = atom_raw.iloc[0::3].reset_index(drop=True)
atom2 = atom_raw.iloc[1::3].reset_index(drop=True)
atom3 = atom_raw.iloc[2::3].reset_index(drop=True)

# =========================
# Plot helper
# Saves PNG and interactive HTML
# =========================
interactive_files = []

def create_plot(filename, title, xlabel, ylabel, series, xlim=None):

    # -------------------------
    # Static Matplotlib plot
    # -------------------------
    fig, ax = plt.subplots(figsize=(10, 4))

    for label, x, y in series:
        ax.plot(x, y, label=label)

    ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.legend(loc="upper right")
    ax.grid(True, alpha=0.3)

    if xlim is not None:
        ax.set_xlim(*xlim)

    ax.text(
        0.02, 0.95,
        f"Δt = {timestep_str} {time_unit}",
        transform=ax.transAxes,
        verticalalignment="top",
        bbox=dict(
            boxstyle="round",
            facecolor="white",
            alpha=0.8
        )
    )

    fig.tight_layout()
    fig.savefig(output_dir / f"{filename}.png", dpi=300)
    plt.close(fig)

    # -------------------------
    # Interactive Plotly plot
    # -------------------------
    interactive_fig = go.Figure()

    for label, x, y in series:
        x = np.asarray(x)
        y = np.asarray(y)

        # Downsample only for interactive display
        stride = max(1, int(np.ceil(len(x) / max_interactive_points)))

        interactive_fig.add_trace(
            go.Scattergl(
                x=x[::stride],
                y=y[::stride],
                mode="lines",
                name=label
            )
        )

    interactive_fig.update_layout(
        title=title,
        xaxis_title=xlabel,
        yaxis_title=ylabel,
        template="plotly_white",
        width=1100,
        height=500,
        hovermode="x unified",
        legend=dict(
            x=1,
            y=1,
            xanchor="right",
            yanchor="top"
        )
    )

    if xlim is not None:
        interactive_fig.update_xaxes(range=list(xlim))

    interactive_fig.add_annotation(
        x=0.02,
        y=0.95,
        xref="paper",
        yref="paper",
        text=f"Δt = {timestep_str} {time_unit}",
        showarrow=False,
        bgcolor="white",
        bordercolor="black",
        borderwidth=1,
        borderpad=5
    )

    html_path = output_dir / f"{filename}.html"

    interactive_fig.write_html(
        html_path,
        include_plotlyjs=True,
        full_html=True
    )

    interactive_files.append(html_path)

# =========================
# Plot 1: Atom displacements
# =========================
create_plot(
    filename="atom_displacements",
    title="Time-dependent displacements for the CO₂ molecule",
    xlabel=f"Time ({time_unit})",
    ylabel="Displacement (Å)",
    series=[
        ("Atom 1 (O)", atom1["time"], atom1["displacement"]),
        ("Atom 2 (C)", atom2["time"], atom2["displacement"]),
        ("Atom 3 (O)", atom3["time"], atom3["displacement"])
    ]
)

# =========================
# Plot 2: Energy data
# =========================
create_plot(
    filename="simulation_energies",
    title="Time-dependence of the energies for the CO₂ molecule",
    xlabel=f"Time ({time_unit})",
    ylabel="Energy (meV)",
    series=[
        ("Total energy", sim_df["time"], sim_df["total"]),
        ("Pot. energy", sim_df["time"], sim_df["potential"]),
        ("Kin. energy", sim_df["time"], sim_df["kinetic"])
    ]
)

# =========================
# Calculate FFT power spectrum
# =========================
displacements = np.column_stack([
    atom1["displacement"].to_numpy(),
    atom2["displacement"].to_numpy(),
    atom3["displacement"].to_numpy()
])

N = len(displacements)

# Remove mean displacement
displacements -= np.mean(displacements, axis=0)

# FFT for all three atoms
fft_result = np.fft.rfft(displacements, axis=0)

# Frequencies in THz (since timestep is in ps)
frequencies = np.fft.rfftfreq(N, d=timestep)

# Squared-amplitude power spectrum
power = np.abs(fft_result)**2 / N**2

# Sum powers of all three atoms
total_power = np.sum(power, axis=1)

# =========================
# Plot 3: Power spectrum
# =========================
frequency_mask = frequencies <= 150

create_plot(
    filename="power_spectrum",
    title="Power spectrum of the CO₂ molecule",
    xlabel="Frequency (THz)",
    ylabel="Power (Å²)",
    series=[
        (
            "Total power spectrum",
            frequencies[frequency_mask],
            total_power[frequency_mask]
        )
    ],
    xlim=(0, 150)
)

# =========================
# Open interactive plots
# =========================
if open_interactive_plots:
    for html_path in interactive_files:
        webbrowser.open(html_path.resolve().as_uri())

# =========================
# Summary
# =========================
print(f"\nPlots saved in: {output_dir.resolve()}")

for filename in [
    "atom_displacements",
    "simulation_energies",
    "power_spectrum"
]:
    print(f"- {filename}.png")
    print(f"- {filename}.html")
