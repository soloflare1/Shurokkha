#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
Preferences prefs;

Adafruit_SSD1306 d(128, 64, &Wire, -1);


int btn = 4;
int buzzer = 5;

int pressCount = 0;
unsigned long lastPressTime = 0;
bool sosExecuted = false;

float lat = 0, lng = 0;


void showHome() {
  d.clearDisplay();
  d.setTextColor(WHITE);

  d.setTextSize(2);
  d.setCursor(10, 0);
  d.println("SHUROKKHA");

  d.setTextSize(1);
  d.setCursor(15, 25);
  d.println("Your Safety");

  d.setCursor(15, 40);
  d.println("Our Priority");

  d.display();
}


void showSilentSOS() {
  d.clearDisplay();
  d.setTextColor(WHITE);

  d.setTextSize(2);
  d.setCursor(25, 15);
  d.println("SILENT");

  d.setTextSize(1);
  d.setCursor(10, 40);
  d.println("Alert Sent Quietly");

  d.display();
}


void showLoudSOS() {
  d.clearDisplay();
  d.setTextColor(WHITE);

  d.setTextSize(2);
  d.setCursor(40, 15);
  d.println("LOUD");

  d.setTextSize(1);
  d.setCursor(5, 40);
  d.println("Emergency Active");

  d.display();
}


bool buttonPressed() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(btn);

  if (lastState == HIGH && currentState == LOW) {
    delay(50);
    lastState = currentState;
    return true;
  }

  lastState = currentState;
  return false;
}


void setup() {

  Serial.begin(115200);

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(btn, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);

  Wire.begin(21, 22);

  if (!d.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  prefs.begin("gps", false);

  lat = prefs.getFloat("lat", 0);
  lng = prefs.getFloat("lng", 0);
}


void loop() {

  // GPS READ
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // GPS LOGIC
  if (gps.location.isValid()) {

    float newLat = gps.location.lat();
    float newLng = gps.location.lng();

    if (newLat != 0 && newLng != 0) {

      lat = newLat;
      lng = newLng;

      prefs.putFloat("lat", lat);
      prefs.putFloat("lng", lng);
    }

  } else {
    lat = prefs.getFloat("lat", lat);
    lng = prefs.getFloat("lng", lng);
  }

  // BUTTON PRESS
  if (buttonPressed()) {
    pressCount++;
    lastPressTime = millis();
    sosExecuted = false;

    Serial.print("Press: ");
    Serial.println(pressCount);
  }

  // NORMAL DISPLAY
  if (pressCount == 0) {
    showHome();
    delay(800);
  }

  // SOS TRIGGER
  if (pressCount > 0 && millis() - lastPressTime > 1200 && !sosExecuted) {

    sosExecuted = true;

    Serial.println("SOS ACTIVATED");

    Serial.print("LAT: ");
    Serial.println(lat, 6);

    Serial.print("LNG: ");
    Serial.println(lng, 6);

    Serial.print("MAP: https://maps.google.com/?q=");
    Serial.print(lat, 6);
    Serial.print(",");
    Serial.println(lng, 6);

    // SILENT MODE
    if (pressCount == 2) {

      Serial.println("MODE: SILENT");

      showSilentSOS();
      digitalWrite(buzzer, LOW);
    }

    // LOUD MODE
    else if (pressCount == 3) {

      Serial.println("MODE: LOUD");

      showLoudSOS();

      for (int i = 0; i < 10; i++) {
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, LOW);
        delay(200);
      }
    }

    delay(2000);
    pressCount = 0;
    sosExecuted = false;
  }
}
