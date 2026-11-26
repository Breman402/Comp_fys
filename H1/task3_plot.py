import argparse
import shutil
from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


def analyze(filename, output_dir, equilibration):
    df = pd.read_csv(filename)

    # Save original simulation data
    shutil.copy2(filename, output_dir / filename)

    name = "equilibration" if equilibration else "constant"

    # Plot simulation data
    fig, ax = plt.subplots(3, 1, sharex=True, figsize=(10, 8))

    ax[0].plot(df["time"], df["temperature"])
    ax[0].axhline(773.15, color="red", linestyle="--")
    ax[0].set_ylabel("Temperature [K]")

    ax[1].plot(df["time"], df["pressure_bar"])
    ax[1].axhline(1.0, color="red", linestyle="--")
    ax[1].set_ylabel("Pressure [bar]")

    ax[2].plot(df["time"], df["lattice_parameter"])
    ax[2].set_ylabel("Lattice parameter [Å]")
    ax[2].set_xlabel("Time [ps]")

    fig.tight_layout()
    fig.savefig(output_dir / f"{name}.pdf")
    plt.close(fig)

    # Calculate statistics
    results = []

    if equilibration:
        results.append("Equilibration data (last 2 ps)")

        data = df[df["time"] >= df["time"].max() - 2.0]

        a_avg = data["lattice_parameter"].mean()
        a_std = data["lattice_parameter"].std()
        a_final = df["lattice_parameter"].iloc[-1]

        results.extend([
            f"Average lattice parameter: {a_avg:.6f} Å",
            f"Lattice parameter standard deviation: {a_std:.6f} Å",
            f"Final lattice parameter: {a_final:.6f} Å"
        ])

    else:
        results.append("Constant energy and volume data")

        data = df
        a = data["lattice_parameter"].iloc[0]

        results.append(f"Lattice parameter: {a:.6f} Å")

    T_avg = data["temperature"].mean()
    P_avg = data["pressure_bar"].mean()

    T_std = data["temperature"].std()
    P_std = data["pressure_bar"].std()

    results.extend([
        f"Average temperature: {T_avg:.4f} K",
        f"Average pressure: {P_avg:.4f} bar",
        f"Temperature standard deviation: {T_std:.4f} K",
        f"Pressure standard deviation: {P_std:.4f} bar"
    ])

    # Print and save statistics
    result_text = "\n".join(results)

    print(result_text)
    print()

    (output_dir / f"{name}_stats.txt").write_text(
        result_text + "\n",
        encoding="utf-8"
    )


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "duration",
        help="Simulation durations in ps, e.g. X-Y where X is the equilibration run and Y is the constant run"
    )

    args = parser.parse_args()

    output_dir = Path("task3_output") / args.duration
    output_dir.mkdir(parents=True, exist_ok=True)

    analyze(
        "H1_task3_equ.csv",
        output_dir,
        equilibration=True
    )

    analyze(
        "H1_task3_const.csv",
        output_dir,
        equilibration=False
    )

    print(f"Results saved to {output_dir}/")


if __name__ == "__main__":
    main()