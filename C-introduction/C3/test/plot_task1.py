import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt("trajectory.dat")

plt.plot(data[:, 1], data[:, 2])

plt.xlabel("x")
plt.ylabel("y")
plt.title("Projectile trajectory")
plt.grid(True)
plt.show()