# SHUROKKHA – Hardware Wiring & Integration

## Hardware Connection Table

| Component            | Pin        | ESP32   | Interface/Function |
|-----------------------|------------|---------|---------------------|
| GPS Module (NEO-6M)   | VCC        | 3.3V    | —                   |
|                       | GND        | GND     | —                   |
|                       | TX         | GPIO 16 | UART                |
|                       | RX         | GPIO 17 | UART                |
| OLED SSD1306 (I2C)    | VCC        | 3.3V    | —                   |
|                       | GND        | GND     | —                   |
|                       | SDA        | GPIO 21 | I2C                 |
|                       | SCL        | GPIO 22 | I2C                 |
| SOS Push Button       | One Side   | GPIO 4  | Digital Input       |
|                       | Other Side | GND     | —                   |
| Active Buzzer         | +          | GPIO 5  | Digital Output      |
|                       | −          | GND     | —                   |

## Wiring Layout

```text
ESP32
│
├── GPIO16 ← GPS TX
├── GPIO17 → GPS RX
│
├── GPIO21 → OLED SDA
├── GPIO22 → OLED SCL
│
├── GPIO4 ← SOS BUTTON
│           ↓
│          GND
│
├── GPIO5 → BUZZER (+)
└── GND → BUZZER (-)
```

## GPS Serial Configuration

```cpp
gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
```

- GPS TX → ESP32 GPIO16
- GPS RX → ESP32 GPIO17

## Required Libraries

```text
TinyGPS++
Adafruit SSD1306
Adafruit GFX
Preferences (ESP32 built-in)
```

## SOS Activation Logic

Detection uses a 1-second press-counting window starting from the first
valid (debounced) button press:

| Press Pattern                     | System Response                                     |
|------------------------------------|------------------------------------------------------|
| 1 press only                       | No alert                                              |
| 2 presses within 1 s, then stop    | Silent Alert                                          |
| 3 presses within 1 s               | Audible Alert + Buzzer                                |
| 4 or 5 presses                     | Audible alert already activated after the 3rd press   |
