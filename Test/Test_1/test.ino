#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>
#include "esp_task_wdt.h"
#include "esp_system.h"

// -------------------- CONFIG --------------------
#define SDA_PIN 8
#define SCL_PIN 9
#define DHT_PIN 4
#define DHT_TYPE DHT22

const char* WIFI_SSID     = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* FIREBASE_URL =
"YOUR_FIREBASE_URL";

const char* OPEN_METEO_URL =
"YOUR_OPEN_METEO_URL";

// -------------------- OBJECTS --------------------
Adafruit_BMP280 bmp;
BH1750 lightMeter;
DHT dht(DHT_PIN, DHT_TYPE);

// -------------------- HELPERS --------------------
bool valid(float x) {
  return !isnan(x) && isfinite(x);
}

void HARD_REBOOT(String reason) {
  Serial.println("REBOOT: " + reason);
  delay(1000);
  ESP.restart();
}

// -------------------- WIFI --------------------
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++retry > 20) HARD_REBOOT("WiFi fail");
  }

  Serial.println("\nWiFi Connected");
}

// -------------------- API --------------------
bool fetchAPI(float &temp, float &hum, float &press) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, OPEN_METEO_URL);

  int code = http.GET();
  if (code != 200) {
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  DynamicJsonDocument doc(10000);
  if (deserializeJson(doc, payload)) return false;

  temp = doc["current_weather"]["temperature"] | NAN;

  JsonArray humArr = doc["hourly"]["relativehumidity_2m"];
  JsonArray presArr = doc["hourly"]["surface_pressure"];

  hum = humArr[0] | NAN;
  press = presArr[0] | NAN;

  return valid(temp) && valid(hum) && valid(press);
}

// -------------------- FIREBASE --------------------
void pushFirebase(String data) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, FIREBASE_URL);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST(data);

  if (code < 200 || code > 299) {
    http.end();
    HARD_REBOOT("Firebase fail");
  }

  http.end();
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);

  // ✅ FIXED WATCHDOG (NEW API)
  esp_task_wdt_config_t wdt_config = {
    .timeout_ms = 30000,
    .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
    .trigger_panic = true
  };

  esp_task_wdt_init(&wdt_config);
  esp_task_wdt_add(NULL);

  // I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // BMP280
  if (!bmp.begin(0x76) && !bmp.begin(0x77)) {
    HARD_REBOOT("BMP not found");
  }

  // BH1750
  if (!lightMeter.begin()) {
    HARD_REBOOT("BH1750 fail");
  }

  // DHT22
  dht.begin();
  delay(2000); // DHT stabilization

  // WiFi
  connectWiFi();

  // Time sync
  configTime(0, 0, "pool.ntp.org");

  Serial.println("System Ready");
}

// -------------------- LOOP --------------------
void loop() {
  esp_task_wdt_reset();

  // -------- READ SENSORS --------
  float tBmp = bmp.readTemperature();
  float pBmp = bmp.readPressure() / 100.0;

  float tDht = dht.readTemperature();
  float hDht = dht.readHumidity();

  float lux = lightMeter.readLightLevel();

  // -------- VALIDATE --------
  if (!valid(tBmp) || !valid(pBmp)) HARD_REBOOT("BMP error");
  if (!valid(tDht) || !valid(hDht)) HARD_REBOOT("DHT error");
  if (!valid(lux)) HARD_REBOOT("BH error");

  // -------- API --------
  float apiT, apiH, apiP;
  if (!fetchAPI(apiT, apiH, apiP)) {
    HARD_REBOOT("API fail");
  }

  // -------- TIME --------
  time_t now = time(nullptr);
  if (now < 100000) HARD_REBOOT("Time fail");

  // -------- BUILD JSON --------
  StaticJsonDocument<1024> doc;

  doc["timestamp"] = now;

  JsonObject raw = doc.createNestedObject("raw");
  raw["bmp_temp"] = tBmp;
  raw["bmp_pressure"] = pBmp;
  raw["dht_temp"] = tDht;
  raw["humidity"] = hDht;
  raw["light"] = lux;

  JsonObject api = doc.createNestedObject("api");
  api["temp"] = apiT;
  api["humidity"] = apiH;
  api["pressure"] = apiP;

  String out;
  serializeJson(doc, out);

  Serial.println(out);

  // -------- PUSH --------
  pushFirebase(out);

  delay(10000);
}
