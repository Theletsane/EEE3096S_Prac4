import numpy as np
import matplotlib.pyplot as plt

# Constants
N = 128  # number of samples
max_val = 4095  # 12-bit resolution (0 to 4095)

# Sine wave - CORRECTED: should go from 0 to 2π for one complete cycle
x = np.linspace(0, 2*np.pi, N, endpoint=False)
sine = ((np.sin(x) + 1) / 2 * max_val).astype(int)

# Sawtooth wave - CORRECTED: should include the endpoint to reach max_val
saw = np.linspace(0, max_val, N, endpoint=True).astype(int)

# Triangle wave - CORRECTED: proper triangle wave generation
triangle = np.concatenate([
    np.linspace(0, max_val, N//2, endpoint=False),  # Rising edge
    np.linspace(max_val, 0, N//2, endpoint=False)   # Falling edge
]).astype(int)

# Print arrays in C format
def to_c_array(name, data):
    print(f"uint16_t {name}[{N}] = {{")
    # Print 8 values per line for better readability
    for i in range(0, len(data), 8):
        line = data[i:i+8]
        print("    " + ", ".join(map(str, line)) + ("," if i+8 < len(data) else ""))
    print("};\n")

print("// Generated LUTs for STM32 DAC (12-bit resolution, 0-4095)")
print("// Number of samples:", N)
print()

to_c_array("Sin_LUT", sine)
to_c_array("Saw_LUT", saw)
to_c_array("Triangle_LUT", triangle)

# Plot to verify waveforms
plt.figure(figsize=(12, 8))

plt.subplot(3, 1, 1)
plt.plot(sine, 'b-', linewidth=2)
plt.title('Sine Wave LUT')
plt.ylabel('Value (0-4095)')
plt.grid(True)
plt.ylim(0, 4095)

plt.subplot(3, 1, 2)
plt.plot(saw, 'r-', linewidth=2)
plt.title('Sawtooth Wave LUT')
plt.ylabel('Value (0-4095)')
plt.grid(True)
plt.ylim(0, 4095)

plt.subplot(3, 1, 3)
plt.plot(triangle, 'g-', linewidth=2)
plt.title('Triangle Wave LUT')
plt.xlabel('Sample Index')
plt.ylabel('Value (0-4095)')
plt.grid(True)
plt.ylim(0, 4095)

plt.tight_layout()
plt.show()

# Verify ranges
print("Verification:")
print(f"Sine range: {sine.min()} to {sine.max()}")
print(f"Sawtooth range: {saw.min()} to {saw.max()}")
print(f"Triangle range: {triangle.min()} to {triangle.max()}")

