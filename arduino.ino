#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ===== LCD SETUP =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== DHT11 SETUP =====
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== ULTRASONIC (HC-SR04) SETUP =====
#define TRIG_PIN 5
#define ECHO_PIN 18

// ===== MQ2 GAS SENSOR SETUP =====
#define MQ2_A_PIN 34
#define MQ2_D_PIN 15

// ===== BUZZER & RGB LED PINS =====
#define BUZZER_PIN 4
#define LED_R_PIN 26
#define LED_G_PIN 25
#define LED_B_PIN 27

// --- MQTT & Wi-Fi Configuration ---
const char* ssid = "IOTtesting";
const char* password = "12345678";
const char* mqtt_server = "broker.hivemq.com"; // Public Test Broker
const int mqtt_port = 1883;
const char* mqtt_topic = "farm/esp32/data";   // Central topic for all sensor data
const char* clientID = "ESP32_Farm_Client";

// Network objects
WiFiClient espClient;
PubSubClient client(espClient);

// Alert thresholds
const float TEMP_MAX = 30.0;
const float TEMP_MIN = 15.0;
const float HUM_MAX = 50.0;
const float HUM_MIN = 40.0;
const float DIST_MIN = 15.0;

// Helper: ultrasonic distance
float readUltrasonicCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return -1.0;
  return duration * 0.0343 / 2.0;
}

// Helper: RGB (ACTIVE-LOW)
void setRGB(bool r, bool g, bool b) {
  digitalWrite(LED_R_PIN, r ? LOW : HIGH);
  digitalWrite(LED_G_PIN, g ? LOW : HIGH);
  digitalWrite(LED_B_PIN, b ? LOW : HIGH);
}

// === Wi-Fi Connection Function ===
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {
    delay(500);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected!");
    lcd.setCursor(0, 1);
    // Convert IP to string then to C-string for LCD
    String ipStr = WiFi.localIP().toString();
    lcd.print(ipStr.substring(0, 16)); // LCD is 16 chars wide
  } else {
    Serial.println("WiFi Failed. Resetting.");
    lcd.clear();
    lcd.print("WiFi Failed!");
    delay(5000);
    ESP.restart();
  }
}

// === MQTT Reconnection Function ===
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting MQTT");
    lcd.setCursor(0, 1);
    lcd.print("Please wait...");

    // Attempt to connect with Client ID
    if (client.connect(clientID)) {
      Serial.println("connected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Connected");
      delay(500);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" - retry in 5s");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT Failed");
      lcd.setCursor(0, 1);
      lcd.print("Retrying in 5s");
      delay(5000); // simple retry delay (blocking)
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup peripherals
  dht.begin();
  lcd.init();
  lcd.backlight();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(MQ2_A_PIN, INPUT);
  pinMode(MQ2_D_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  setRGB(false, false, false);

  // Network setup
  client.setServer(mqtt_server, mqtt_port);
  setup_wifi();

  delay(1000);
  lcd.clear();
}

void loop() {
  // Maintain Network Status
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Required for MQTT to work

  // --- 1. Read Sensors ---
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float distance = readUltrasonicCm();
  int gasAnalog = analogRead(MQ2_A_PIN);
  bool gasDetected = (digitalRead(MQ2_D_PIN) == LOW);

  // --- 2. Alerts ---
  bool validTH = !(isnan(h) || isnan(t));
  bool tempInRange = (t >= TEMP_MIN && t <= TEMP_MAX);
  bool humInRange = (h >= HUM_MIN && h <= HUM_MAX);

  // FIXED: alert if either temp OR humidity is out of range (previously used AND)
  bool thAlert = (validTH && (!tempInRange || !humInRange));
  bool distAlert = (distance > 0 && distance < DIST_MIN);
  bool gasAlert = gasDetected;

  int alertCount = (thAlert ? 1 : 0) + (distAlert ? 1 : 0) + (gasAlert ? 1 : 0);

  // LED & Buzzer Control
  if (alertCount == 0 && validTH && t < TEMP_MAX && h < HUM_MAX) {
    setRGB(false, true, false);
  } else if (alertCount >= 2) {
    setRGB(true, false, false);
  } else if (alertCount == 1) {
    setRGB(true, true, false);
  } else {
    setRGB(false, false, false);
  }

  if (alertCount == 1) {
    digitalWrite(BUZZER_PIN, HIGH); delay(200);
    digitalWrite(BUZZER_PIN, LOW);  delay(300);
  } else if (alertCount == 2) {
    for (int i = 0; i < 2; i++) {
      digitalWrite(BUZZER_PIN, HIGH); delay(150);
      digitalWrite(BUZZER_PIN, LOW);  delay(150);
    }
  } else if (alertCount >= 3) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER_PIN, HIGH); delay(80);
      digitalWrite(BUZZER_PIN, LOW);  delay(80);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // === JSON Serialization and MQTT Publish ===
  StaticJsonDocument<256> doc; // Create the JSON document

  // Add all sensor data and alert status
  doc["t"] = t;
  doc["h"] = h;
  doc["d"] = distance;
  doc["gasA"] = gasAnalog;
  doc["alerts"] = alertCount;
  doc["gas_safe"] = !gasAlert; // Simple safety boolean

  char jsonBuffer[256];
  size_t n = serializeJson(doc, jsonBuffer, sizeof(jsonBuffer));

  // Publish the JSON message
  if (client.publish(mqtt_topic, jsonBuffer, n)) {
    Serial.print("MQTT published: ");
    Serial.println(jsonBuffer);
  } else {
    Serial.println("MQTT Publish FAILED.");
  }

  // --- LCD Display (show status) ---
  lcd.setCursor(0, 0);
  if (!validTH) {
    lcd.print("DHT11 ERROR     ");
  } else {
    // Compose a short first line: T:xx.xC H:xx%
    char line1[17];
    snprintf(line1, sizeof(line1), "T:%2.1f%cC H:%2.0f%%", t, 223, h);
    lcd.print(line1);
    // pad to clear any leftover chars
    int len = strlen(line1);
    for (int i = len; i < 16; i++) lcd.print(' ');
  }

  lcd.setCursor(0, 1);
  // Second line: distance and gas status
  char line2[17];
  if (distance < 0 || distance > 400) {
    snprintf(line2, sizeof(line2), "D:---cm %s", gasAlert ? "GAS!" : "AirOK");
  } else {
    snprintf(line2, sizeof(line2), "D:%3.0fcm %s", distance, gasAlert ? "GAS!" : "AirOK");
  }
  lcd.print(line2);
  // pad
  int len2 = strlen(line2);
  for (int i = len2; i < 16; i++) lcd.print(' ');

  delay(500); // Sampling delay
}
