# STM32 Audio Waveform Generator with PWM-based DAC

## Project Overview

This project implements a digital-to-analog audio system on an STM32F4 microcontroller using PWM (Pulse Width Modulation) as a "quick and dirty" DAC. The system generates audio waveforms from lookup tables (LUTs) and plays them through a speaker via an amplifier circuit. Users can cycle through different waveforms using a pushbutton, with the current selection displayed on an LCD screen.

## Features

- **6 Different Waveforms:**
  - 3 Basic Waveforms: Sine, Sawtooth, Triangle
  - 3 Audio Samples: Piano, Guitar, Drum (sampled at 44.1 kHz)
  
- **Hardware PWM-based DAC:**
  - Uses TIM3 to generate PWM signal
  - 12-bit resolution (0-4095 values)
  - Duty cycle controlled by DMA for efficient operation

- **DMA Transfer:**
  - Automatic transfer of LUT values to PWM duty cycle register
  - No CPU intervention during waveform generation
  - Circular buffer mode for continuous playback

- **Button Control:**
  - Cycles through waveforms with pushbutton (PA0)
  - Debouncing implemented (200ms threshold)
  - Interrupt-driven for responsive switching

- **LCD Display:**
  - Shows current waveform name
  - Updates automatically when waveform changes

## Hardware Requirements

- STM32F4 Development Board (e.g., STM32F407 Discovery)
- Breadboard
- Small speaker (8Ω recommended)
- Amplifier circuit components:
  - Op-amp (e.g., LM358, TL072)
  - Transistors (for power amplification)
  - Resistors and capacitors
- Potentiometer (for volume control - optional)
- 16x2 LCD Display (4-bit mode)
- Pushbutton (connected to PA0)
- Jumper wires

## Pin Configuration

### LCD Pins (4-bit mode):
- **RS (Register Select):** PC14
- **E (Enable):** PC15
- **D4:** PB8
- **D5:** PB9
- **D6:** PA12
- **D7:** PA15

### Timer/PWM:
- **TIM3_CH3 PWM Output:** Configured in .ioc file (typically PA6, PB0, or PC8)

### Button:
- **Button0:** PA0 (with pull-up resistor, rising edge interrupt)

## System Architecture

### Timer Configuration

**TIM2 (Sampling Timer):**
- Operates in Output Compare mode
- Generates trigger events at calculated frequency
- Period: `TIM2_Ticks = TIM2CLK / (NS × F_signal)`
- Triggers DMA transfer on each update event

**TIM3 (PWM Generator):**
- Operates in PWM mode on Channel 3
- Period: 65535 (16-bit counter)
- Duty cycle dynamically controlled by CCR3 register
- Output frequency: ~244 Hz (16 MHz / 65536)

### DMA Configuration

- **Stream:** DMA1_Stream5
- **Channel:** DMA_CHANNEL_3 (TIM2_CH1)
- **Direction:** Memory to Peripheral
- **Source:** LUT arrays (Sin_LUT, Saw_LUT, etc.)
- **Destination:** TIM3->CCR3 register
- **Mode:** Circular (automatically wraps around)
- **Data Width:** 32-bit (WORD)

### Signal Flow

```
LUT Array → DMA → TIM3->CCR3 → PWM Signal → Low-Pass Filter → Amplifier → Speaker
            ↑
         Triggered by TIM2 at calculated rate
```

## Key Parameters

```c
#define NS 128              // Number of samples in LUT
#define TIM2CLK 16000000    // Timer clock: 16 MHz
#define F_SIGNAL 440        // Output frequency: 440 Hz (A4 note)
#define TIM2_Ticks (TIM2CLK / (NS * F_SIGNAL))  // ~284 ticks
```

### Frequency Calculation

For a desired output frequency `F_signal`:
- Each complete LUT cycle produces one waveform period
- TIM2 must trigger `NS` times per period
- Therefore: `TIM2_Frequency = NS × F_signal`
- Timer period: `TIM2_Ticks = TIM2CLK / TIM2_Frequency`

**Example (440 Hz sine wave):**
- TIM2 must trigger at: 128 × 440 = 56,320 Hz
- TIM2 period: 16,000,000 / 56,320 ≈ 284 clock cycles

### Frequency Limits

**Maximum F_signal:**
- Limited by TIM2CLK and NS
- Max = TIM2CLK / (NS × 2) ≈ 62.5 kHz (Nyquist)
- Practical max: ~20 kHz (human hearing limit)

**Minimum F_signal:**
- Limited by 32-bit timer counter maximum
- Min ≈ 1 Hz (practically limited by perception)

## Implementation Details

### Task 1: Lookup Tables (LUTs)
All LUTs contain 128 samples with 12-bit resolution (0-4095).

**Generation formulas:**
- **Sine:** `value = 2047.5 × (1 + sin(2πi/128))`
- **Sawtooth:** `value = (4095 × i) / 127`
- **Triangle:** Ramps from 0→4095 (first half), then 4095→0 (second half)
- **Audio files:** Processed from .wav files using Python scripts

### Task 2: Parameter Configuration
```c
NS = 128           // Samples per LUT
TIM2CLK = 16 MHz   // From system clock configuration
F_SIGNAL = 440 Hz  // Desired output frequency (A4 musical note)
```

### Task 3: Timer Tick Calculation
```c
TIM2_Ticks = TIM2CLK / (NS × F_SIGNAL)
           = 16,000,000 / (128 × 440)
           = 284 (approximately)
```

### Task 4: DMA and Timer Initialization
In `main()`:
```c
// Start PWM generation
HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

// Start sampling timer
HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_1);

// Configure DMA transfer
HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Sin_LUT, DestAddress, NS);

// Initialize LCD
init_LCD();
lcd_command(CLEAR);
lcd_putstring("Sine");

// Enable DMA trigger from TIM2
__HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
```

### Task 5: Button Interrupt Handler
```c
void EXTI0_IRQHandler(void) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // Debouncing: 200ms threshold
    if (current_time - last_interrupt_time > 200) {
        last_interrupt_time = current_time;
        
        // Stop current DMA
        __HAL_TIM_DISABLE_DMA(&htim2, TIM_DMA_CC1);
        HAL_DMA_Abort_IT(&hdma_tim2_ch1);
        
        // Cycle to next waveform (0→1→2→3→4→5→0)
        current_waveform = (current_waveform + 1) % 6;
        
        // Restart DMA with new LUT
        switch(current_waveform) {
            case 0: /* Sine */
            case 1: /* Sawtooth */
            case 2: /* Triangle */
            case 3: /* Piano */
            case 4: /* Guitar */
            case 5: /* Drum */
        }
        
        // Re-enable DMA
        __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
    }
    
    HAL_GPIO_EXTI_IRQHandler(Button0_Pin);
}
```

## How It Works

1. **Initialization Phase:**
   - System clocks configured (16 MHz HSI)
   - TIM2 configured for Output Compare mode
   - TIM3 configured for PWM generation
   - DMA configured for circular memory-to-peripheral transfer
   - GPIO configured for button interrupt and LCD control

2. **Waveform Generation:**
   - TIM2 counts up at 16 MHz
   - When counter reaches `TIM2_Ticks - 1`, it triggers DMA
   - DMA reads next value from LUT array
   - DMA writes value to TIM3->CCR3
   - TIM3 PWM duty cycle instantly changes
   - Process repeats 128 times per waveform cycle

3. **PWM to Analog Conversion:**
   - PWM signal has varying duty cycle (0-100%)
   - Low-pass filter (RC circuit) averages PWM signal
   - Output is proportional to LUT value: V_out ≈ V_cc × (LUT_value / 4095)
   - Amplifier boosts signal to drive speaker

4. **User Interaction:**
   - Button press triggers EXTI0 interrupt
   - Interrupt handler debounces input
   - Current DMA transfer stopped
   - New LUT selected based on `current_waveform` counter
   - DMA restarted with new source address
   - LCD updated with waveform name

## Building and Flashing

1. **Open Project:**
   ```bash
   cd Practical4
   # Import existing makefile project in STM32CubeIDE
   ```

2. **Build:**
   - Click Build button or press `Ctrl+B`
   - Verify no compilation errors

3. **Flash to Board:**
   - Connect ST-LINK programmer
   - Click Debug/Run button
   - Code automatically flashed and executed

## Testing and Verification

### Verification Steps:

1. **Power On:**
   - LCD should display "Sine"
   - Speaker should produce 440 Hz tone

2. **Button Test:**
   - Press button once → "Sawtooth" displayed
   - Press again → "Triangle"
   - Continue through Piano, Guitar, Drum
   - Press again → returns to "Sine"

3. **Audio Verification:**
   - Use oscilloscope to view PWM output
   - Should see varying duty cycle at ~244 Hz carrier frequency
   - Low-pass filtered output should match expected waveform shape

4. **Frequency Measurement:**
   - Measure output frequency with oscilloscope
   - Should be approximately 440 Hz for basic waveforms
   - Audio waveforms will have complex frequency content

## Python Scripts

### `generate_lut.py`
Generates basic waveform LUTs (sine, sawtooth, triangle) with visualization.

**Usage:**
```bash
python generate_lut.py
```

**Output:**
- Matplotlib plots showing waveform shapes
- C array declarations printed to console

### `read_from_wave.py`
Converts .wav audio files to LUT arrays.

**Usage:**
```bash
python read_from_wave.py
```

**Features:**
- Reads .wav files at any sample rate
- Converts stereo to mono
- Normalizes amplitude to 12-bit range
- Resamples to 128 samples
- Generates plots and C arrays
- Saves output to `LUT_arrays.txt`

## Troubleshooting

| Problem | Possible Cause | Solution |
|---------|---------------|----------|
| No sound | Amplifier circuit issue | Check connections, verify PWM output on scope |
| Distorted sound | Incorrect LUT values | Verify LUT generation, check for clipping |
| Button not responding | Debouncing issue | Increase debounce delay, check pull-up resistor |
| LCD not updating | Timing issue | Check LCD initialization, verify GPIO configuration |
| Wrong frequency | TIM2_Ticks calculation | Recalculate based on TIM2CLK and F_SIGNAL |
| Choppy audio | DMA not circular | Verify DMA mode is set to DMA_CIRCULAR |

## Theory: PWM as a DAC

PWM works as a DAC through time-averaging. For a PWM signal with:
- Period: T
- High time: t_high
- Duty cycle: D = t_high / T

The average voltage is: **V_avg = V_cc × D**

For our 12-bit LUT values (0-4095):
- Duty cycle = LUT_value / 4095
- V_avg = 3.3V × (LUT_value / 4095)

By changing the duty cycle faster than the ear can detect (~20 kHz), and using a low-pass filter, we create a smooth analog voltage that represents our audio waveform.

## Performance Characteristics

- **Sample Rate:** 56.32 kHz (NS × F_SIGNAL)
- **Bit Depth:** 12-bit (4096 levels)
- **PWM Frequency:** ~244 Hz (carrier)
- **CPU Load:** Minimal (DMA handles transfers)
- **Latency:** ~3.5 μs per sample
- **Max Audio Frequency:** ~28 kHz (Nyquist limit)

## Future Improvements

- [ ] Add volume control using ADC and potentiometer
- [ ] Implement frequency adjustment via additional buttons
- [ ] Add more audio samples (bass, violin, synth)
- [ ] Implement audio effects (reverb, delay, distortion)
- [ ] Add UART interface for real-time waveform upload
- [ ] Optimize LUT size for memory efficiency
- [ ] Implement proper anti-aliasing filter

## References

- STM32F4 Reference Manual (RM0090)
- STM32F4 HAL Driver Documentation
- Digital Signal Processing fundamentals
- PWM-based DAC theory

## License

Educational project for EEE3096S - Embedded Systems II
University of Cape Town, 2025

## Authors

[Your Student Numbers]
Group Members: [Names]

---

**Last Updated:** 2025-10-04