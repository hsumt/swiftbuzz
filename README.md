https://docs.google.com/document/d/e/2PACX-1vSE1X3inybETFW6bcxJyrkRjBECvuXUSWEriFJHz58BMzkm-Ny_3w0cbavtWdUfDjhPFHPW-Zip6t80/pub
# AWANA Electronic Quizzing System Interface

A custom Arduino Mega + web application interface for the AWANA Electronic Quizzing System (EQS). The system captures buzzer presses, records reaction times, ranks contestants, and displays results on both a browser-based scoreboard and an OLED display.

## Features

* Supports up to 10 buzzers by default
* Expandable to 20 buzzers
* Real-time ranking and reaction timing
* Browser-based scoreboard (no server required)
* OLED display showing winner and rankings
* Simple USB serial communication
* Compatible with Arduino Mega 2560

---

# Hardware

## Required Components

* Arduino Mega 2560 (or compatible clone)
* SSD1306 128x64 I2C OLED Display
* AWANA Electronic Quizzing System
* DB25 Y-Splitter Cable
* DB25 Breakout Board
* USB-B Cable
* Jumper Wires

---

# Wiring

## Buzzers (Port 1)

| DB25 Pin | Arduino Pin | Buzzer |
| -------- | ----------- | ------ |
| 1        | 22          | 1      |
| 2        | 23          | 2      |
| 3        | 24          | 3      |
| 4        | 25          | 4      |
| 5        | 26          | 5      |
| 6        | 27          | 6      |
| 7        | 28          | 7      |
| 8        | 29          | 8      |
| 9        | 30          | 9      |
| 10       | 31          | 10     |
| GND      | GND         | Ground |

All buzzer inputs use `INPUT_PULLUP`, meaning pins normally read HIGH and become LOW when pressed.

## OLED Display

| OLED Pin | Arduino Mega |
| -------- | ------------ |
| VCC      | 5V           |
| GND      | GND          |
| SDA      | 20           |
| SCL      | 21           |

Default I2C address: `0x3C`

---

# Software

## Arduino

Install the following libraries:

* Adafruit GFX
* Adafruit SSD1306

Default configuration:

```cpp
const int NUM_BUZZERS = 10;
const int MAX_RANKED = 5;
```

---

## Serial Protocol

### Browser → Arduino

| Command | Description    |
| ------- | -------------- |
| R       | Reset round    |
| Q       | Start question |

### Arduino → Browser

| Message       | Description                            |
| ------------- | -------------------------------------- |
| BUZZ          | Buzzer pressed                         |
| LOCKED        | Maximum rankings reached               |
| QUESTION_TIME | Time from question start to first buzz |
| RESET         | Round reset                            |

---

# Web Interface

## Setup

1. Open `index.html` in Chrome or Edge.
2. Click **Connect**.
3. Select the Arduino COM port.
4. Enter participant names (optional).
5. Click **Start Question**.
6. Contestants buzz in.
7. Click **Reset Round** for the next question.

### Notes

* Names are stored locally in the browser.
* No internet connection or server is required.
* Firefox does not support Web Serial and is not compatible.

---

# OLED Display

The OLED shows:

* First-place buzzer number
* Winner reaction time
* Top 5 rankings

The display updates automatically after each buzz and reset.

---

# Expansion to 20 Buzzers

The EQS provides two DB25 ports (1–10 and 11–20).

Add:

* Second DB25 breakout board
* Second DB25 Y-splitter

Update the Arduino configuration:

```cpp
const int NUM_BUZZERS = 20;

const int buzzerPins[NUM_BUZZERS] = {
  22,23,24,25,26,27,28,29,30,31,
  32,33,34,35,36,37,38,39,40,41
};
```

Add buzzer name fields 11–20 to the HTML interface. No JavaScript changes are required.

---

# Troubleshooting

| Problem                   | Solution                                       |
| ------------------------- | ---------------------------------------------- |
| Cannot connect            | Verify USB cable, drivers, and use Chrome/Edge |
| OLED not detected         | Check SDA/SCL wiring and try address `0x3D`    |
| Buzzers not registering   | Verify GND connection and pin mapping          |
| Incorrect names displayed | Check HTML `data-buzzer` values                |
| Random buzz on startup    | Ensure EQS is powered before connecting USB    |

---

# Usage

| Button         | Function                                |
| -------------- | --------------------------------------- |
| Connect        | Connect browser to Arduino              |
| Start Question | Begin timing and enable buzzing         |
| Stop Timer     | Stop timer without clearing results     |
| Reset Round    | Clear results and prepare next question |

---

### Future Improvements

* Team scoring
* Tournament mode
* Export results to CSV
* Wireless buzzer support
* Multiple display layouts

This project was designed to modernize the AWANA Electronic Quizzing System while preserving compatibility with existing hardware.
