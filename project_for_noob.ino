#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// ====== PINS ======
#define ADC_PIN 34
#define BUZZER_PIN 25
#define ADC_SAMPLES 1500

// ====== LCD ======
LiquidCrystal_PCF8574 lcd(0x27);   // change to 0x3F if needed

// ====== WIFI ======
const char* ssid = "NASA";
const char* password = "W-gv_12444456;%";
const char* webhook = "https://discord.com/api/webhooks/1476148118054633656/WXdGqPwuqP1wCsGBJqA6g4XdXb76fvLQ27KefduO-j32zvlWFKMxLCPqoW-UHnEBjgTg";

// ====== SETTINGS ======
float calibrationFactor = 0.0;  
float alertThreshold = 5.0;      

bool alertSent = false;

// =========================================

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  // I2C LCD
  Wire.begin(21, 22);
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  // Buzzer (new ESP32 LEDC style)
  ledcAttach(BUZZER_PIN, 2000, 8);

  lcd.setCursor(0,0);
  lcd.print("Power Monitor");
  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  delay(1500);
  lcd.clear();
}

// =========================================

void loop() {

  float sum = 0;
  float offset = 0;

  // --- Calculate offset ---
  for (int i = 0; i < ADC_SAMPLES; i++) {
    offset += analogRead(ADC_PIN);
  }
  offset /= ADC_SAMPLES;

  // --- Calculate RMS ---
  for (int i = 0; i < ADC_SAMPLES; i++) {
    float sample = analogRead(ADC_PIN) - offset;
    sum += sample * sample;
  }

  float rms = sqrt(sum / ADC_SAMPLES);
  float voltage = (rms / 4095.0) * 3.3;
  float current = voltage * calibrationFactor;

  Serial.print("Current: ");
  Serial.println(current);

  // ---- LCD ----
  lcd.setCursor(0,0);
  lcd.print("Current:");
  lcd.setCursor(0,1);
  lcd.print(current,2);
  lcd.print(" A     ");

  // ---- OVERLOAD ----
  if (current > alertThreshold && !alertSent) {

    sendDiscordAlert(current);
    beepAlarm();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("!! OVERLOAD !!");
    lcd.setCursor(0,1);
    lcd.print(current,2);
    lcd.print(" A");

    delay(2000);
    lcd.clear();

    alertSent = true;
  }

  if (current < alertThreshold - 0.5) {
    alertSent = false;
  }

  delay(1000);
}

// =========================================
// BUZZER FUNCTIONS (Passive)

void beep(int frequency, int duration) {
  ledcWriteTone(BUZZER_PIN, frequency);
  delay(duration);
  ledcWriteTone(BUZZER_PIN, 0);
}

void beepAlarm() {
  beep(1500, 200);
  delay(100);
  beep(2500, 200);
  delay(100);
  beep(1500, 200);
  delay(100);
  beep(2500, 400);
}

// =========================================
// DISCORD FUNCTION

void sendDiscordAlert(float current) {

  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  if (https.begin(client, webhook)) {

    https.addHeader("Content-Type", "application/json");

    String payload = "{\"content\":\"⚠️ Overcurrent Detected! Current: "
                     + String(current,2) + " A\"}";

    int httpCode = https.POST(payload);

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    https.end();
  }
}