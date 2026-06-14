# SHUROKKHA – ESP32, GPS, OLED, SOS Integration

## Hardware Connection Table

| Component           | Pin        | ESP32   |
| ------------------- | ---------- | ------- |
| GPS Module (NEO-6M) | VCC        | 3.3V    |
|                     | GND        | GND     |
|                     | TX         | GPIO 16 |
|                     | RX         | GPIO 17 |
| OLED SSD1306 (I2C)  | VCC        | 3.3V    |
|                     | GND        | GND     |
|                     | SDA        | GPIO 21 |
|                     | SCL        | GPIO 22 |
| SOS Push Button     | One Side   | GPIO 4  |
|                     | Other Side | GND     |
| Active Buzzer       | +          | GPIO 26 |
|                     | −          | GND     |

---

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
├── GPIO26 → BUZZER (+)
└── GND → BUZZER (-)
```

---

## GPS Serial Configuration

```cpp
gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
```

### Meaning

* GPS TX → ESP32 GPIO16
* GPS RX → ESP32 GPIO17

---

## Required Libraries

```text
TinyGPS++
Adafruit SSD1306
Adafruit GFX
Preferences (ESP32 built-in)
```

---

## SOS Features

| Button Press | Action                 |
| ------------ | ---------------------- |
| 0 Press      | Home Screen + GPS Save |
| 2 Presses    | Silent SOS             |
| 3 Presses    | Loud SOS + Buzzer      |
