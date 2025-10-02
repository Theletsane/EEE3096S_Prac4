import numpy as np
import matplotlib.pyplot as plt

# Number of samples
N = 128
def generate_Sin_lut():
    # Generate sine wave LUT
    sine_lut = []
    for i in range(N):
        # YOUR CODE HERE: calculate sine value
        # Hint: use np.sin() and np.pi
        value = 2047.5 * (1 + np.sin(2*np.pi*i/128))# ... your formula
        sine_lut.append(int(round(value)))

    # Plot to verify
    plt.plot(sine_lut)
    plt.title("Sine Wave LUT")
    plt.xlabel("Sample Index")
    plt.ylabel("DAC Value (0-4095)")
    plt.grid(True)
    plt.show()

    # Print as C array
    print("uint16_t sine_LUT[128] = {")
    for i, val in enumerate(sine_lut):
        print(f"    {val},", end="")
        if (i+1) % 10 == 0:
            print()  # New line every 10 values
    print("\n};")

def generate_SawTooth_lut():
    # Generate sawtooth wave LUT
    sawtooth_lut = []
    for i in range(N):
        # YOUR CODE HERE: calculate sawtooth value
        value = int(round((4095 * i) / (N - 1))) # ... your formula
        sawtooth_lut.append(value)

    # Plot to verify
    plt.plot(sawtooth_lut)
    plt.title("Sawtooth Wave LUT")
    plt.xlabel("Sample Index")
    plt.ylabel("DAC Value (0-4095)")
    plt.grid(True)
    plt.show()

    # Print as C array
    print("uint16_t sawtooth_LUT[128] = {")
    for i, val in enumerate(sawtooth_lut):
        print(f"    {val},", end="")
        if (i+1) % 10 == 0:
            print()  # New line every 10 values
    print("\n};")

def generate_Triangle_lut():
    # Generate triangle wave LUT
    triangle_lut = []
    for i in range(N):
        # YOUR CODE HERE: calculate triangle value
        if i < N/2:
            value = int(round((4095/(N/2))*i)) # ... your formula
        else:
            value = int(round(4095 - (4095/(N/2))*(i - N/2))) # ... your formula
        triangle_lut.append(value)

    # Plot to verify
    plt.plot(triangle_lut)
    plt.title("Triangle Wave LUT")
    plt.xlabel("Sample Index")
    plt.ylabel("DAC Value (0-4095)")
    plt.grid(True)
    plt.show()

    # Print as C array
    print("uint16_t triangle_LUT[128] = {")
    for i, val in enumerate(triangle_lut):
        print(f"    {val},", end="")
        if (i+1) % 10 == 0:
            print()  # New line every 10 values
    print("\n};")

#generate_SawTooth_lut()
generate_Triangle_lut()

