# SHUROKKHA (শুরক্ষা)
### Smart IoT-Based Wearable Device for Real-Time Personal Safety

SHUROKKHA is an ESP32-based wearable personal safety device that lets a user
trigger an emergency alert with a simple button press. On activation, it
fetches the current GPS location and automatically sends an emergency email
(with a Google Maps link to the user's location) to a pre-configured
emergency contact — with both a **Loud (audible) SOS** mode and a
**Silent SOS** mode for situations where the user cannot communicate openly.

---

## ✨ Features

- **Two SOS modes**, selected by number of button presses:
  - **2 presses → Silent SOS** — sends the alert quietly, no buzzer.
  - **3 presses → Loud SOS** — sounds the buzzer and sends the alert.
- **Real-time GPS tracking** via the NEO-6M GPS module, with the last known
  location persisted to flash (`Preferences`) so it survives power loss /
  no GPS fix.
- **Automated email alerts** over WiFi using Gmail SMTP
  (`ESP_Mail_Client`), including a direct Google Maps link to the location.
- **OLED status display** (SSD1306) showing system state: initializing,
  connecting, ready, sending, sent/failed.
- **Auto-reconnect WiFi** — retries in the background without blocking the
  main loop.
- **Battery powered & portable** — 18650 Li-ion cell with TP4056 charging
  and a boost converter for stable operating voltage.

---

## 🔧 Hardware

| Component | Role |
|---|---|
| ESP32 DevKit (ESP-WROOM-32) | Main controller — runs firmware, WiFi, GPS parsing, display, buzzer |
| NEO-6M GPS Module | Provides real-time latitude/longitude |
| OLED Display (SSD1306, 1.3") | Shows system status |
| SOS Push Button | Emergency input trigger |
| Buzzer Module | Audible alert (Loud SOS mode) |
| 18650 Li-ion Battery | Portable power source |
| TP4056 Charging Module | Battery charge management/protection |
| Boost Converter (LM2596-style) | Steps battery voltage up to required operating voltage |
| Breadboard & Jumper Wires | Prototype circuit assembly |

<p align="center">
  <img src="docs/images/07_esp32_devkit.png" width="220" alt="ESP32 DevKit"/>
  <img src="docs/images/01_neo6m_gps.png" width="220" alt="NEO-6M GPS Module"/>
  <img src="docs/images/03_oled_display.png" width="220" alt="OLED Display"/>
</p>
<p align="center">
  <img src="docs/images/02_sos_button.png" width="140" alt="SOS Button"/>
  <img src="docs/images/05_buzzer.png" width="140" alt="Buzzer"/>
  <img src="docs/images/04_18650_battery.png" width="140" alt="18650 Battery"/>
  <img src="docs/images/06_boost_converter.png" width="220" alt="Boost Converter"/>
</p>

---

## 📸 Demo

<p align="center">
  <img src="docs/images/08_demo_loud_sos.png" width="260" alt="Loud SOS triggered, sending alert"/>
  <img src="docs/images/09_demo_silent_sos.png" width="260" alt="Silent SOS triggered, sending alert"/>
  <img src="docs/images/10_demo_email_sent.png" width="260" alt="Email sent confirmation on OLED"/>
</p>

<p align="center">
  <img src="docs/images/11_full_assembly.jpeg" width="600" alt="Full hardware assembly with received alert email on phone"/>
</p>

<p align="center">
  <img src="docs/images/12_gps_location_verified.png" width="600" alt="Received location verified on Google Maps at BAIUST campus"/>
</p>

---

## 📖 Publication

This project is documented in an IEEE-format conference paper:
**"SHUROKKHA: A Wi-Fi and GPS Based Wearable Personal Safety System with Multi-Press SOS Activation."**

> **Abstract** — SHUROKKHA is an IoT-based wearable safety device built
> around the ESP32 microcontroller that provides emergency alerts and
> GPS-based location information. Rather than a dedicated GSM module, it
> uses the ESP32's built-in Wi-Fi and Gmail SMTP to email a pre-configured
> contact when an SOS is triggered. A multi-press button mechanism selects
> between a Silent alert mode and an Audible (buzzer) alert mode, and the
> current GPS fix — or the last valid stored fix, if none is available —
> is included as a Google Maps link. Testing on physical hardware confirmed
> correct recognition of the SOS press patterns and successful delivery of
> location-linked emergency emails.

**Why no GSM module?** Compared to related GPS/GSM-based safety systems
(SMS alerts over cellular network), SHUROKKHA trades cellular independence
for lower hardware complexity and cost — it relies on Wi-Fi + Gmail SMTP
instead. The trade-off is that it needs internet access to send an alert.

The full six-page IEEE two-column paper (Word format) is not included in
this repo to keep it firmware-focused — add it under `docs/` if you'd like
it version-controlled here as well.

---

## 🛠️ Firmware Setup

This project uses the Arduino framework for ESP32.

### 1. Required Libraries
Install via the Arduino Library Manager:
- `TinyGPSPlus`
- `Adafruit GFX Library`
- `Adafruit SSD1306`
- `ESP Mail Client` (mobizt)
- `Preferences` (bundled with ESP32 board package)

### 2. Configure credentials
Credentials are **kept out of version control**. To set yours up:

```bash
cp firmware/config.h.example firmware/config.h
```

Then edit `firmware/config.h` with your own WiFi SSID/password and Gmail
credentials. For the email password, use a **Gmail App Password**
(not your real account password) — generate one at
[myaccount.google.com/apppasswords](https://myaccount.google.com/apppasswords).

> ⚠️ `firmware/config.h` is listed in `.gitignore` and will never be committed.
> Never commit real credentials to a public repository.

### 3. Wiring (default pins)

| ESP32 Pin | Connects to |
|---|---|
| GPIO 16 (RX) | GPS TX |
| GPIO 17 (TX) | GPS RX |
| GPIO 21 | OLED SDA |
| GPIO 22 | OLED SCL |
| GPIO 4 | SOS push button (other leg → GND) |
| GPIO 5 | Buzzer (+) |

Full wiring diagram and pin table: [docs/WIRING.md](docs/WIRING.md)

### 4. Flash it
Open `firmware/shurokkha.ino` in the Arduino IDE, select your ESP32 board, and
upload.

---

## ⚙️ How It Works

1. On boot, the device connects to WiFi and the OLED shows **"Ready"**.
2. `readGPS()` continuously parses NMEA data from the GPS module; the last
   valid fix is cached in flash so a location is available even without a
   fresh fix.
3. `readButton()` debounces the SOS button and counts presses within a
   1-second window:
   - 2 presses → Silent SOS
   - 3 presses → Loud SOS
4. `sos()` updates the OLED, optionally sounds the buzzer, and calls
   `sendEmailAlert()`, which connects to Gmail SMTP and sends a formatted
   alert email containing a Google Maps link to the current/last-known
   location.
5. `maintainWiFi()` silently retries the WiFi connection in the background
   if it drops.

---

## 📄 License

This project is released under the MIT License — see [LICENSE](LICENSE).
