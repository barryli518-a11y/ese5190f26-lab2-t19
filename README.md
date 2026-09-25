# ESE 5190 Lab 2: Morse Code Decoder

## Team Members

- Name:
- Name:

## Part A: LEDs and Pushbutton

### S1: Blinking LEDs

<!-- Add image here -->

### S2: Button Controlled LED

<!-- Add image here -->

### S3: Cycling LEDs

<!-- Add image here -->

### S4: Circuit Schematic

<!-- Add image here -->

## Part B: Interrupts

The program uses Timer1 input capture interrupts on PB0. The built-in LED turns on when the button is pressed and turns off when it is released.

### Stop & Think

Interrupts allow the CPU to perform other work while waiting for an input. Polling continuously uses CPU time. Interrupts require more setup, and switch bounce can cause multiple interrupt events.

## Part C: Dash or Dot

### R1

For a 16 MHz clock:

- 50 ms = 800,000 ticks
- 200 ms = 3,200,000 ticks
- 400 ms = 6,400,000 ticks

### R2

A prescaler divides the clock before it reaches the timer. This allows the timer to measure longer intervals before overflowing, but reduces timing resolution.

## Part D: Hardware Debouncing

### I1: Circuit

<!-- Add image here -->

### I2: 1 nF Measurement

<!-- Add image here -->

### I3: 10 nF Measurement

<!-- Add image here -->

### R3

<!-- Add response here -->

## Part E: Morse Decoder

<!-- Add notes or images here -->

## Part F: Morse Encoder

<!-- Add notes or images here -->
