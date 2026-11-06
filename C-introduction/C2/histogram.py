"""Plot the vector written by C2 to build/vector.csv."""

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


def main():
    # Resolve paths relative to this script so it works from any directory.
    build_dir = Path(__file__).resolve().parents[2] / "build"
    csv_path = build_dir / "vector.csv"
    if not csv_path.is_file():
        raise SystemExit(
            f"Could not find {csv_path}. Run the C2 executable from the build folder first."
        )

    values = np.loadtxt(csv_path, delimiter=",", ndmin=1)
    if values.size == 0:
        raise SystemExit(f"No values found in {csv_path}.")

    fig, ax = plt.subplots()
    ax.hist(values, bins="auto", edgecolor="black")
    ax.set_title("Distribution of vector values")
    ax.set_xlabel("Value")
    ax.set_ylabel("Frequency")
    fig.tight_layout()

    output_path = build_dir / "vector_histogram.png"
    fig.savefig(output_path, dpi=150)
    print(f"Histogram saved to {output_path}")
    plt.show()


if __name__ == "__main__":
    main()
