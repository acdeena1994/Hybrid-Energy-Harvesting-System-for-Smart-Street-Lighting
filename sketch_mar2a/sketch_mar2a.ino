#include <WiFi.h>
#include <HTTPClient.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // Common I2C address for SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions for I2C
#define I2C_SCL 22
#define I2C_SDA 21

// WiFi credentials
const char* ssid = "ACD";
const char* password = "pass1234";

// Flask server URL
const char* serverUrl = "http://192.168.43.251:5000/data";

// Pin definitions
#define LDR_PIN 35
#define PIR_PIN 23
#define RELAY_PIN 5
#define LED_PIN 4
#define CURRENT_SENSOR_PIN 34
#define BATTERY_VOLTAGE_PIN 32
#define SOLAR_VOLTAGE_PIN 33

// Constants
const float REF_VOLTAGE = 3.3;
const int MAX_ADC = 4095;
const float VOLTAGE_DIVIDER_FACTOR = 2.0;
const float ACS712_SENSITIVITY = 185.0;
const float ACS712_REF_VOLTAGE = 2.5;

// Task handles
TaskHandle_t Task1;
TaskHandle_t Task2;

// Sensor data structure
struct SensorData {
    int ldrValue;
    int pirValue;
    float current;
    float batteryVoltage;
    int batteryLevel;
    float solarVoltage;
};

SensorData sensorData;

// Relay control variables
unsigned long relayStartTime = 0;
bool relayActive = false;

// Function prototypes
void readBatteryVoltage();
void readSolarVoltage();
float readCurrent();
void sensorTask(void* pvParameters);
void httpTask(void* pvParameters);
void checkWiFiConnection();
String createJsonPayload();
void displayRoboEye();
void displayCharging();

void setup() {
    Serial.begin(115200);

    // Initialize I2C and OLED
    Wire.begin(I2C_SDA, I2C_SCL);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    
    // Display team name "HexTech"
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 25);
    display.println("HexTech");
    display.display();
    delay(2000); // Show team name for 2 seconds

    // Initialize pins
    pinMode(LDR_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(CURRENT_SENSOR_PIN, INPUT);
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Create tasks
    xTaskCreatePinnedToCore(sensorTask, "SensorTask", 10000, NULL, 1, &Task1, 0);
    xTaskCreatePinnedToCore(httpTask, "HttpTask", 10000, NULL, 1, &Task2, 1);
}

void loop() {
    // Empty loop as tasks handle the functionality
}

// Read Battery Voltage & Percentage
void readBatteryVoltage() {
    int adcValue = analogRead(BATTERY_VOLTAGE_PIN);
    float voltage = (adcValue / (float)MAX_ADC) * REF_VOLTAGE * VOLTAGE_DIVIDER_FACTOR;
    int level = map(voltage * 100, 200, 370, 0, 100);
    level = constrain(level, 0, 100);

    sensorData.batteryVoltage = voltage;
    sensorData.batteryLevel = level;
}

// Read Solar Voltage
void readSolarVoltage() {
    int adcValue = analogRead(SOLAR_VOLTAGE_PIN);
    float voltage = (adcValue / (float)MAX_ADC) * REF_VOLTAGE * VOLTAGE_DIVIDER_FACTOR;
    sensorData.solarVoltage = voltage;
}

// Read Current from ACS712
float readCurrent() {
    int adcValue = analogRead(CURRENT_SENSOR_PIN);
    float voltage = (adcValue / (float)MAX_ADC) * REF_VOLTAGE;
    float current = (voltage - ACS712_REF_VOLTAGE) * 1000 / ACS712_SENSITIVITY;
    return current;
}

// Sensor task to read all values and control OLED
void sensorTask(void* pvParameters) {
    while (1) {
        // Read sensor values
        sensorData.ldrValue = analogRead(LDR_PIN);
        sensorData.pirValue = digitalRead(PIR_PIN);
        sensorData.current = readCurrent();
        readBatteryVoltage();
        readSolarVoltage();

        // Print all sensor data to Serial Monitor
        Serial.println("==== Sensor Readings ====");
        Serial.print("LDR Value: ");
        Serial.println(sensorData.ldrValue);
        Serial.print("PIR Value: ");
        Serial.println(sensorData.pirValue);
        Serial.print("Current: ");
        Serial.print(sensorData.current, 2);
        Serial.println(" mA");
        Serial.print("Battery Voltage: ");
        Serial.print(sensorData.batteryVoltage, 2);
        Serial.println(" V");
        Serial.print("Battery Level: ");
        Serial.print(sensorData.batteryLevel);
        Serial.println(" %");
        Serial.print("Solar Voltage: ");
        Serial.print(sensorData.solarVoltage, 2);
        Serial.println(" V");
        Serial.println("======================");

        // OLED Display logic based on solar voltage
        display.clearDisplay();
        if (sensorData.solarVoltage <= 1.0) {
            displayRoboEye();
        } else {
            displayCharging();
        }
        display.display();

        // Control LED based on LDR value
        if (sensorData.ldrValue <= 2000) {
            digitalWrite(LED_PIN, HIGH);
            if (sensorData.pirValue == HIGH && !relayActive) {
                digitalWrite(RELAY_PIN, LOW);
                relayActive = true;
                relayStartTime = millis();
            }
        } else {
            digitalWrite(LED_PIN, LOW);
            digitalWrite(RELAY_PIN, HIGH);
            relayActive = false;
        }

        if (relayActive && (millis() - relayStartTime >= 5000)) {
            digitalWrite(RELAY_PIN, HIGH);
            relayActive = false;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// HTTP Task for sending data to Flask server
void httpTask(void* pvParameters) {
    while (1) {
        checkWiFiConnection();
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");

            String jsonData = createJsonPayload();
            int httpResponseCode = http.POST(jsonData);
            
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
            } else {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }

            http.end();
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Check WiFi connection
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected, reconnecting...");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Reconnected to WiFi");
    }
}

// Create JSON payload
String createJsonPayload() {
    StaticJsonDocument<200> doc;
    doc["ldr"] = sensorData.ldrValue;
    doc["pir"] = sensorData.pirValue;
    doc["current"] = sensorData.current;
    doc["battery_voltage"] = sensorData.batteryVoltage;
    doc["battery_level"] = sensorData.batteryLevel;
    doc["solar_voltage"] = sensorData.solarVoltage;

    String jsonData;
    serializeJson(doc, jsonData);
    Serial.print("Sending JSON data: ");
    Serial.println(jsonData);
    return jsonData;
}

// Robo Eye Animation
void displayRoboEye() {
    static uint8_t eyePos = 0;
    display.drawCircle(64, 32, 20, SSD1306_WHITE); // Eye outline
    int x = 64 + (eyePos % 3 - 1) * 5; // Simple left-right movement
    display.fillCircle(x, 32, 8, SSD1306_WHITE); // Pupil
    eyePos++;
    if (eyePos > 2) eyePos = 0;
}

// Charging Animation
void displayCharging() {
    static uint8_t chargeLevel = 0;
    display.drawRect(40, 20, 48, 24, SSD1306_WHITE); // Battery outline
    display.fillRect(88, 26, 4, 12, SSD1306_WHITE); // Battery tip
    int width = map(chargeLevel, 0, 3, 0, 44);
    display.fillRect(42, 22, width, 20, SSD1306_WHITE); // Charging bar
    chargeLevel = (chargeLevel + 1) % 4;
}