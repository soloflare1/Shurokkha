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
int buzzer = 26;

int pressCount = 0;
unsigned long lastPress = 0;
unsigned long lastOledUpdate = 0;

float lat = 0, lng = 0;
float lastLat = 0, lastLng = 0;

//  HOME SCREEN 
void showHome() {

  d.clearDisplay();
  d.setTextColor(WHITE);

  d.setTextSize(2);
  d.setCursor(10, 0);
  d.println("SHUROKKHA");

  d.setTextSize(1);
  d.setCursor(0, 25);

  d.print("DATE: 29/5/2026");

  d.setCursor(0, 40);
  d.print("TIME: 04:40");

  d.display();
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

  d.clearDisplay();
  d.display();

  prefs.begin("gps", false);

  lastLat = prefs.getFloat("lat", 0);
  lastLng = prefs.getFloat("lng", 0);
}


void loop() {

  
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {

    lat = gps.location.lat();
    lng = gps.location.lng();

    if (abs(lat - lastLat) > 0.00001 || abs(lng - lastLng) > 0.00001) {

      prefs.putFloat("lat", lat);
      prefs.putFloat("lng", lng);

      lastLat = lat;
      lastLng = lng;
    }
  }

  // btn
  if (digitalRead(btn) == LOW) {
    delay(200);
    pressCount++;
    lastPress = millis();
    while (digitalRead(btn) == LOW);
  }

  // NO PRESS -> ONLY LOCATION
  if (pressCount == 0) {

    Serial.print("LAT: ");
    Serial.println(lat, 6);

    Serial.print("LNG: ");
    Serial.println(lng, 6);
  }

  // HOME SCREEN 
  if (pressCount == 0 && millis() - lastOledUpdate > 1000) {
    showHome();
    lastOledUpdate = millis();
  }

  // SOS MODE
  if (pressCount > 0 && millis() - lastPress > 2000) {

    
    if (pressCount == 2 || pressCount == 3) {

      Serial.println("HELP 🚨");

      Serial.print("LAT: ");
      Serial.println(lat, 6);

      Serial.print("LNG: ");
      Serial.println(lng, 6);

      Serial.print("MAP: https://maps.google.com/?q=");
      Serial.print(lat, 6);
      Serial.print(",");
      Serial.println(lng, 6);
    }

    if (pressCount == 2) {

      d.clearDisplay();
      d.setTextColor(WHITE);

      d.setTextSize(1);
      d.setCursor(35, 15);
      d.println("SILENT");

      d.setTextSize(2);
      d.setCursor(45, 35);
      d.println("SOS");

      d.display();

      digitalWrite(buzzer, LOW);
    }

    
    else if (pressCount == 3) {

      d.clearDisplay();
      d.setTextColor(WHITE);

      d.setTextSize(1);
      d.setCursor(40, 15);
      d.println("LOUD");

      d.setTextSize(2);
      d.setCursor(45, 35);
      d.println("SOS");

      d.display();

      for (int i = 0; i < 10; i++) {
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, LOW);
        delay(200);
      }
    }

    delay(2500);
    pressCount = 0;
  }
}
