# Task 1 & 2 - Waveform Generation

This folder contains the completed work for Task 1 and Task 2 of EEE3096S Practical 4.

## Files:
- `main.c` - Updated main.c with LUTs and constants
- `generate_luts.py` - Python script for basic waveforms (sine, sawtooth, triangle)
- `fixed_wav_processing.py` - Python script for processing .wav files
- `drum.wav`, `guitar.wav`, `piano.wav` - Audio files

## Task 1:
- Generated LUTs for sinusoid, sawtooth, and triangular waves
- Generated LUTs for 3 sound waveforms from .wav files
- All LUTs have 128 values in range 0-4095 (12-bit resolution)

## Task 2:
- Defined constants: NS=128, TIM2CLK=16000000, F_SIGNAL=1000
- Calculated TIM2_Ticks = 125
- Provided reasoning for F_SIGNAL limits based on Nyquist theorem
