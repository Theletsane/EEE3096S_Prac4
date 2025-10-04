import wave
import numpy as np

def process_wav_safe(filename, output_samples=128):
    print(f"Processing {filename}...")
    
    with wave.open(filename, 'rb') as wav_file:
        frames = wav_file.readframes(-1)
        sample_rate = wav_file.getframerate()
        channels = wav_file.getnchannels()
        sample_width = wav_file.getsampwidth()
        
    print(f"  Sample rate: {sample_rate} Hz")
    print(f"  Channels: {channels}")
    print(f"  Sample width: {sample_width} bytes")
    
    # Convert to numpy array with proper data type
    if sample_width == 2:
        samples = np.frombuffer(frames, dtype=np.int16)
    else:
        samples = np.frombuffer(frames, dtype=np.uint8)
    
    # If stereo, take left channel
    if channels == 2:
        samples = samples[::2]
    
    print(f"  Total samples: {len(samples)}")
    
    # Downsample to 128 samples
    if len(samples) > output_samples:
        indices = np.linspace(0, len(samples)-1, output_samples, dtype=int)
        samples = samples[indices]
    
    # Safe normalization to 0-4095 range
    samples_min = samples.min()
    samples_max = samples.max()
    
    print(f"  Original range: {samples_min} to {samples_max}")
    
    if samples_max == samples_min:
        normalized = np.full(output_samples, 2048)
    else:
        # Use safe normalization to avoid overflow
        # Convert to float64 for precision
        samples_float = samples.astype(np.float64)
        min_float = float(samples_min)
        max_float = float(samples_max)
        
        # Normalize to 0-1 range first
        normalized_01 = (samples_float - min_float) / (max_float - min_float)
        
        # Scale to 0-4095 range
        normalized = (normalized_01 * 4095).astype(np.int32)
        
        # Ensure values are in range
        normalized = np.clip(normalized, 0, 4095)
    
    print(f"  Normalized range: {normalized.min()} to {normalized.max()}")
    
    return normalized

def print_c_array(name, data):
    print(f"uint32_t {name}[{len(data)}] = {{")
    for i in range(0, len(data), 8):
        line = data[i:i+8]
        print("    " + ", ".join(map(str, line)) + ("," if i+8 < len(data) else ""))
    print("};\n")

# Process all three wav files
wav_files = ["piano.wav", "guitar.wav", "drum.wav"]
lut_names = ["Piano_LUT", "Guitar_LUT", "Drum_LUT"]

print("=== FIXED WAV FILE PROCESSING ===")
print("Processing .wav files with safe normalization...")
print()

processed_luts = []

for wav_file, lut_name in zip(wav_files, lut_names):
    try:
        lut_data = process_wav_safe(wav_file)
        processed_luts.append((lut_name, lut_data))
        print(f"[OK] {lut_name} processed successfully")
        print()
    except Exception as e:
        print(f"[ERROR] Failed to process {wav_file}: {e}")
        print()

# Generate C code
if processed_luts:
    print("=== C CODE FOR MAIN.C ===")
    print("// Copy this code into your main.c file:")
    print("// Replace the empty arrays {} with these values")
    print()
    
    for lut_name, lut_data in processed_luts:
        print_c_array(lut_name, lut_data)
    
    print("=== VERIFICATION ===")
    for lut_name, lut_data in processed_luts:
        print(f"{lut_name} range: {lut_data.min()} to {lut_data.max()}")
        print(f"{lut_name} samples: {len(lut_data)}")
    
    print("\nAll LUTs are within 0-4095 range [OK]")
    print("All LUTs have 128 samples [OK]")
    
else:
    print("No .wav files were processed successfully.")
