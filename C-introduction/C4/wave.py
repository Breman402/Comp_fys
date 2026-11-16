import numpy as np
import matplotlib.pyplot as plt

# Frequency (f): Controls how many oscillations occur per time unit.
# Increasing f makes the wave oscillate faster (shorter period).
# Decreasing f makes the wave oscillate slower (longer period).

# Phase (phi): Controls the horizontal shift of the wave.
# Increasing phi shifts the wave to the left (earlier in time).
# For phi = pi/2, the cosine becomes a negative sine wave.

# Amplitude (a): Controls the height of the wave.

a = 2
f = 2
phi = 0
dt = 0.1
N = 250

t = np.arange(N) * dt

h = a * np.cos(2 * np.pi * f * t + phi)

plt.plot(t, h)
plt.xlabel("Time")
plt.ylabel("h(t)")
plt.grid()
plt.show()

# Calculate the discrete Fourier transform of the signal
fft_result = np.fft.fft(h)

# Calculate the power spectrum (squared magnitude of the FFT)
power_spectrum = np.abs(fft_result)**2 / N

# Generate the corresponding frequency values
frequencies = np.fft.fftfreq(N, d=dt)

# Convert FFT bin indices to frequencies from 0 to the sampling frequency
frequencies = np.arange(N) / (N * dt)

plt.plot(frequencies, power_spectrum)
plt.xlabel("Frequency")
plt.ylabel("Power spectrum")
plt.grid()
plt.show()

# Task: Fourier transform of two combined cosine signals

a1 = 1
a2 = 1
f1 = 2
f2 = 6
phi1 = 0
phi2 = 0

dt = 0.1
N = 250

t = np.arange(N) * dt

# Generate a signal consisting of two cosine waves with different frequencies.
h = a1 * np.cos(2 * np.pi * f1 * t + phi1) + \
    a2 * np.cos(2 * np.pi * f2 * t + phi2)

# Plot the combined signal in the time domain.
plt.plot(t, h)
plt.xlabel("Time")
plt.ylabel("h(t)")
plt.grid()
plt.show()

# The Nyquist frequency is half the sampling frequency.
# It represents the highest frequency that can be correctly reconstructed
# from the sampled signal (for frequencies strictly below this limit).
# With dt = 0.1, the Nyquist frequency is 5.
nyquist_frequency = 1 / (2 * dt)
print(f"Nyquist frequency: {nyquist_frequency}")

# Calculate the FFT and power spectrum.
fft_result = np.fft.fft(h)
power_spectrum = np.abs(fft_result)**2 / N

# Generate frequency values corresponding to the FFT bins.
frequencies = np.fft.fftfreq(N, d=dt)

# Shift the spectrum so that negative frequencies appear to the left
# and positive frequencies to the right of zero.
shifted_frequencies = np.fft.fftshift(frequencies)
shifted_power = np.fft.fftshift(power_spectrum)

# Plot the symmetric power spectrum.
plt.plot(shifted_frequencies, shifted_power)
plt.xlabel("Frequency")
plt.ylabel("Power spectrum")
plt.grid()
plt.show()

# OBSERVATIONS:
# Since f2 = 6 exceeds the Nyquist frequency of 5, aliasing occurs.
# This means the sampled signal cannot distinguish f = 6 from f = 4.
# As a result, the power spectrum shows peaks at +/-2 and +/-4,
# instead of the actual frequencies +/-2 and +/-6.
#
# SOLUTION:
# Reduce dt from 0.1 to 0.05 to increase the sampling frequency.
# This increases the Nyquist frequency from 5 to 10.
# Both frequencies (2 and 6) are now below the Nyquist limit,
# so the power spectrum correctly shows peaks at +/-2 and +/-6.
#
# Note that keeping N = 250 while reducing dt also reduces the total
# sampled time from 25 to 12.5, which changes the frequency resolution.