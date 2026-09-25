
# Lab 2 Morse

**Team Name/Number: 19**

**GitHub Repository URL: https://github.com/barryli518-a11y/ese5190f26-lab2-t19.git **

| Team Member Name | Email Address       | GitHub Handle       |
|------------------|---------------------|---------------------|
| [Eric Yao]         | [yaoe@engineering.upenn.edu]           | [eric-yaoza]          |
| [Zikang Li]         | [zikangl@engineering.upenn.edu]           | [barryli518-a11y]          |



## Part A: LEDs and Pushbutton

### S1: Blinking LEDs
![alt text](lab2s1-1.png)
### S2: Button Controlled LED
![alt text](lab2s2.png)

### S3: Cycling LEDs
![alt text](lab2s3.png)

### S4: Circuit Schematic
![alt text](lab2s4.png)

## Part B: Interrupts

# (C1) 
The program uses Timer1 input capture interrupts on PB0. The built-in LED turns on when the button is pressed and turns off when it is released.

Code is attached in lab2c1.c

## Part C: Dash or Dot

### R1

For a 16 MHz clock:

N = f * t

50 ms: 16000000 * 0.050 = 800,000 ticks 

200 ms = 16,000,000 × 0.200 = 3,200,000 ticks

400 ms = 16,000,000 × 0.400 = 6,400,000 ticks


### R2

A prescaler divides the clock before it reaches the timer. This allows the timer to measure longer intervals before overflowing (allows us to measure much lower frequencies), but reduces timing resolution.

## Part D: Hardware Debouncing

### I1: Circuit
![alt text](lab2i1.jpg)

### I2: 1 nF Measurement
![alt text](lab2i2.jpg)

### I3: 10 nF Measurement
![alt text](lab2i3.jpg)

### R3

<!-- Add response here -->

## Part E: Morse Decoder

<!-- Add notes or images here -->

## Part F: Morse Encoder

<!-- Add notes or images here -->
