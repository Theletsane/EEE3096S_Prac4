import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
import os

def process_wav_to_lut(wav_file_path, num_samples=128, output_name="Audio"):
    """
    Convert a .wav file to a lookup table (LUT) for STM32 DAC
    
    Parameters:
    - wav_file_path: path to the .wav file
    - num_samples: number of samples in the LUT (default 128)
    - output_name: name for the output LUT variable
    """
    
    # Read the WAV file
    sample_rate, data = wavfile.read(wav_file_path)
    
    print(f"\n{'='*60}")
    print(f"Processing: {os.path.basename(wav_file_path)}")
    print(f"{'='*60}")
    print(f"Sample Rate: {sample_rate} Hz")
    print(f"Data shape: {data.shape}")
    print(f"Data type: {data.dtype}")
    
    # Handle stereo audio - convert to mono by averaging channels
    if len(data.shape) > 1:
        data = np.mean(data, axis=1)
        print("Converted stereo to mono")
    
    # Normalize to -1.0 to 1.0 range
    if data.dtype == np.int16:
        data = data / 32768.0
    elif data.dtype == np.int32:
        data = data / 2147483648.0
    elif data.dtype == np.uint8:
        data = (data - 128) / 128.0
    else:
        # Assume already normalized
        data = data / np.max(np.abs(data))
    
    print(f"Normalized data range: [{np.min(data):.3f}, {np.max(data):.3f}]")
    
    # Resample to num_samples using linear interpolation
    original_length = len(data)
    indices = np.linspace(0, original_length - 1, num_samples)
    resampled_data = np.interp(indices, np.arange(original_length), data)
    
    # Scale to 12-bit range (0 to 4095)
    # First shift from [-1, 1] to [0, 1]
    scaled_data = (resampled_data + 1.0) / 2.0
    # Then scale to [0, 4095]
    lut_values = np.clip(scaled_data * 4095, 0, 4095).astype(np.uint32)
    
    print(f"LUT range: [{np.min(lut_values)}, {np.max(lut_values)}]")
    
    # Plot the waveform
    plt.figure(figsize=(12, 6))
    
    plt.subplot(2, 1, 1)
    time_original = np.linspace(0, len(data) / sample_rate, len(data))
    plt.plot(time_original[:min(5000, len(data))], data[:min(5000, len(data))])
    plt.title(f'{output_name} - Original Waveform (first 5000 samples)')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    
    plt.subplot(2, 1, 2)
    plt.plot(lut_values, 'o-', markersize=3)
    plt.title(f'{output_name} - LUT Values ({num_samples} samples)')
    plt.xlabel('Sample Index')
    plt.ylabel('12-bit Value (0-4095)')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig(f'{output_name}_LUT.png', dpi=150)
    print(f"Saved plot as {output_name}_LUT.png")
    plt.show()
    
    return lut_values

def format_c_array(lut_values, array_name):
    """
    Format LUT values as a C array declaration
    """
    output = f"uint32_t {array_name}[NS] = {{\n"
    
    # Format 10 values per line
    for i in range(0, len(lut_values), 10):
        line_values = lut_values[i:i+10]
        formatted_values = ', '.join([f'{val:5d}' for val in line_values])
        output += f"    {formatted_values},\n"
    
    # Remove trailing comma and close array
    output = output.rstrip(',\n') + '\n};\n'
    
    return output

# Main execution
if __name__ == "__main__":
    print("WAV to LUT Converter for STM32 DAC")
    print("=" * 60)
    
    # List of wav files to process
    # MODIFY THESE PATHS to match your actual .wav file locations
    wav_files = [
        ("C:/Users/thele/Downloads/piano.wav", "Piano_LUT"),
        ("C:/Users/thele/Downloads/guitar.wav", "Guitar_LUT"),
        ("C:/Users/thele/Downloads/drum.wav", "Drum_LUT")
    ]

    # Process each file
    all_luts = []
    for wav_path, lut_name in wav_files:
        if os.path.exists(wav_path):
            lut_values = process_wav_to_lut(wav_path, num_samples=128, output_name=lut_name)
            c_array = format_c_array(lut_values, lut_name)
            all_luts.append(c_array)
            print(f"\n{lut_name} C Declaration:")
            print(c_array)
        else:
            print(f"\n⚠️  WARNING: File not found: {wav_path}")
            print("Please update the file path in the script")
    
    # Save all LUTs to a text file
    if all_luts:
        with open('LUT_arrays.txt', 'w') as f:
            f.write("// Generated LUT Arrays for STM32\n")
            f.write("// Copy these into your main.c file\n\n")
            for lut in all_luts:
                f.write(lut + "\n")
        print("\n" + "="*60)
        print("✓ All LUTs saved to 'LUT_arrays.txt'")
        print("✓ Copy the contents into your main.c file")
        print("="*60)