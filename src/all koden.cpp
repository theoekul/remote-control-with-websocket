/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------


#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
//#include <Wire.cpp>
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"

// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------
#define LED_PIN 4
#define BTN_PIN 0
#define NEO_PIN 38
#define NEO_COUNT 1
#define HTTP_PORT 80

// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// Button debouncing
const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds

// WiFi credentials
const char *WIFI_SSID = "your_ssid_name";
const char *WIFI_PASS = "your_ssid_password";

// ----------------------------------------------------------------------------
// Definition of the LED component
// ----------------------------------------------------------------------------

struct Led {
    // state variables
    uint8_t pin;
    bool    on;

    // methods
    void update() {
        digitalWrite(pin, on ? HIGH : LOW); // Update the LED state
    }
};

// ----------------------------------------------------------------------------
// Definition of the Button component
// ----------------------------------------------------------------------------

struct Button {
    // state variables
    uint8_t  pin;
    bool     lastReading;
    uint32_t lastDebounceTime;
    uint16_t state;

    // methods determining the logical state of the button
    bool pressed()                { return state == 1; }
    bool released()               { return state == 0xffff; }
    bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

    // method for reading the physical state of the button
    void read() {
        // reads the voltage on the pin connected to the button
        bool reading = digitalRead(pin);

        // if the logic level has changed since the last reading,
        // we reset the timer which counts down the necessary time
        // beyond which we can consider that the bouncing effect
        // has passed.
        if (reading != lastReading) {
            lastDebounceTime = millis();
        }

        // from the moment we're out of the bouncing phase
        // the actual status of the button can be determined
        if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
            // don't forget that the read pin is pulled-up
            bool pressed = reading == LOW;
            if (pressed) {
                if (state < 0xfffe) state++;
                else if (state == 0xfffe) state = 2;
            } else if (state) {
                state = state == 0xffff ? 0 : 0xffff;
            }
        }

        // finally, each new reading is saved
        lastReading = reading;
    }
};

// ----------------------------------------------------------------------------
// Definition of global variables
// ----------------------------------------------------------------------------

Led    led         = { LED_PIN, false };
Button button      = { BTN_PIN, HIGH, 0, 0 };
Adafruit_NeoPixel strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800); // Initialize NeoPixel strip
AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
  }
}

// ----------------------------------------------------------------------------
// Connecting to the WiFi network
// ----------------------------------------------------------------------------

void initWiFi() { // This function connects to the WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 30000) { // 30 seconds timeout
      Serial.print("-");
    delay(500);  // Delay for half a second
  }
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed to connect to WiFi");
      return;
  }
Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
led.on = !led.on;
if (led.on) {
    strip.setPixelColor(0, 100, 0, 0);
} else {
    strip.setPixelColor(0, 0, 0, 0);
}        
notifyClients();
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

String processor(const String &var) { // This function processes variables to be sent to the client
    return String(var == "STATE" && led.on ? "on" : "off");
}

void onRootRequest(AsyncWebServerRequest *request) { // This function sends index.html to the client
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer() { // This function sets up the web server
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients() { // This function sends messages to all clients connected to the WebSocket
   JsonDocument json(200);
    json["status"] = led.on ? "on" : "off";
    json["status_vu"] = analogRead(A0);

    char buffer[200];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) { // This function handles messages from the WebSocket
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        StaticJsonDocument<200> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        const char *action = json["action"]; // This switch statement handles the different actions that can be performed
        if (strcmp(action, "toggle") == 0) {
            led.on = !led.on;
            if (led.on) {
                strip.setPixelColor(0, 100, 0, 0);
            } else {
                strip.setPixelColor(0, 0, 0, 0);
            }   
            notifyClients();
        }

    }
}

void onEvent(AsyncWebSocket       *server, // This function handles events from the WebSocket
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) { // This switch statement handles the different types of events that can occur
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void initStrip() { // This function initializes the NeoPixel strip
    strip.begin();
    strip.show();
}

// ----------------------------------------------------------------------------
// Rotary Switch and LED Control
// ----------------------------------------------------------------------------

#define ROTR_01 8
#define ROTR_02 3
#define ROTR_03 46
#define ROTR_04 9
#define ROTR_05 10
#define ROTR_06 11
#define ROTR_07 12
#define ROTR_08 13

void initRotRead() {
    pinMode(ROTR_01,  INPUT);
    pinMode(ROTR_02,  INPUT);
    pinMode(ROTR_03,  INPUT);
    pinMode(ROTR_04,  INPUT);
    pinMode(ROTR_05,  INPUT);
    pinMode(ROTR_06,  INPUT);
    pinMode(ROTR_07,  INPUT);
    pinMode(ROTR_08,  INPUT);
}

void readRotarySwitch() {
    String position = "Ingen posisjon";
    delay(50); // debounce delay
    if (digitalRead(ROTR_01) == HIGH) position = "Sør"; // 1
    else if (digitalRead(ROTR_02) == HIGH) position = "Sørvest"; // 2
    else if (digitalRead(ROTR_03) == HIGH) position = "Vest"; // 3
    else if (digitalRead(ROTR_04) == HIGH) position = "Nordvest"; // 4
    else if (digitalRead(ROTR_05) == HIGH) position = "Nord"; // 5
    else if (digitalRead(ROTR_06) == HIGH) position = "Nordøst"; // 6
    else if (digitalRead(ROTR_07) == HIGH) position = "Øst"; // 7
    else if (digitalRead(ROTR_08) == HIGH) position = "Sørøst"; // 8
    else position = "No position detected";
    Serial.print("Rotary switch position: ");
    Serial.println(position);
}


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

unsigned long previousMillis = 0;
int previousAnalog = 0;
int meas = 0; // Assuming meas is an integer, you can change the type if needed

void setup() {
    // Initialize Serial monitor for debugging
    Serial.begin(115200);
    Serial.printf("Init I2C");
    Wire.begin(21, 47);
    Wire.beginTransmission(0x74);
    Wire.write(0x06);  // CONFIG Port 0
    Wire.write(0x00);  // Set all OUTPUT
    Wire.endTransmission();
    Wire.beginTransmission(0x74);
    Wire.write(0x07);  // CONFIG Port 1
    Wire.write(0x00);  // Set all OUTPUT
    Wire.endTransmission();
    Serial.printf("Init I2C DONE!");

    pinMode(led.pin, OUTPUT);
    pinMode(button.pin, INPUT);
    pinMode(NEO_PIN, OUTPUT);

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
    // initSWRDisplay(); // Commented out as it is undefined
    initRotRead();
    initStrip();

    strip.setPixelColor(0, 0, 50, 0);
    strip.show();
    delay(1000);  // Wait for configuration
}

#define LED_PIN_1 12
#define LED_PIN_2 11
#define LED_PIN_3 10
#define LED_PIN_4 7
#define LED_PIN_5 14
#define LED_PIN_6 15
#define LED_PIN_7 16
#define LED_PIN_8 17

void loop() {
    ws.cleanupClients();

    if (digitalRead(ROTR_01) == HIGH) {
        digitalWrite(LED_PIN_1, HIGH); // S
    } else {
        digitalWrite(LED_PIN_1, LOW);
    }

    if (digitalRead(ROTR_02) == HIGH) {
        digitalWrite(LED_PIN_2, HIGH); // SW
    } else {
        digitalWrite(LED_PIN_2, LOW);
    }

    if (digitalRead(ROTR_03) == HIGH) {
        digitalWrite(LED_PIN_3, HIGH); // W
    } else {
        digitalWrite(LED_PIN_3, LOW);
    }

    if (digitalRead(ROTR_04) == HIGH) {
        digitalWrite(LED_PIN_4, HIGH); // NW
    } else {
        digitalWrite(LED_PIN_4, LOW);
    }

    if (digitalRead(ROTR_05) == HIGH) {
        digitalWrite(LED_PIN_5, HIGH); // N
    } else {
        digitalWrite(LED_PIN_5, LOW);
    }

    if (digitalRead(ROTR_06) == HIGH) {
        digitalWrite(LED_PIN_6, HIGH); // NE
    } else {
        digitalWrite(LED_PIN_6, LOW);
    }

    if (digitalRead(ROTR_07) == HIGH) {
        digitalWrite(LED_PIN_7, HIGH); // E
    } else {
        digitalWrite(LED_PIN_7, LOW);
    }

    if (digitalRead(ROTR_08) == HIGH) {
        digitalWrite(LED_PIN_8, HIGH); // SE
    } else {
        digitalWrite(LED_PIN_8, LOW);
    }

    // Update the LED strip based on the rotary switch position
    strip.clear();
    if (digitalRead(ROTR_01) == HIGH) strip.setPixelColor(0, 255, 0, 0); // Red for S
    else if (digitalRead(ROTR_02) == HIGH) strip.setPixelColor(0, 255, 165, 0); // Orange for SW
    else if (digitalRead(ROTR_03) == HIGH) strip.setPixelColor(0, 255, 255, 0); // Yellow for W
    else if (digitalRead(ROTR_04) == HIGH) strip.setPixelColor(0, 0, 255, 0); // Green for NW
    else if (digitalRead(ROTR_06) == HIGH) strip.setPixelColor(0, 75, 0, 130); // Indigo for NE
    else if (digitalRead(ROTR_06) == HIGH) strip.setPixelColor(0, 75, 0, 130); // Indigo for NE
    else if (digitalRead(ROTR_07) == HIGH) strip.setPixelColor(0, 238, 130, 238); // Violet for E
    else if (digitalRead(ROTR_08) == HIGH) strip.setPixelColor(0, 255, 255, 255); // White for SE
    else if (digitalRead(ROTR_08) == HIGH) strip.setPixelColor(0, 255, 255, 255); // White for SE
    strip.show();
    button.read();

    // Call the function to read the rotary switch position
    delay(500); // Adjust the delay as needed
    delay(500); // Adjust the delay as needed;

    if (button.pressed()) {
        Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
        led.on = !led.on;
        if (led.on) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 200) {
            previousMillis = currentMillis;
            notifyClients();
            previousAnalog = meas;
        }
        strip.setPixelColor(0, 0, 0, 0);
        notifyClients();
    }
    delay(500);
    }

    
    
    if ((meas > previousAnalog + 30) || (meas < previousAnalog - 30)) { // Denne delen av koden er for å oppdatere SWR displayet
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 200) {
            // save the last time you blinked the LED
            previousMillis = currentMillis;
            
            notifyClients();
            previousAnalog = meas;
        }
    }

    delay(500);  // Delay for half a second
    strip.show();
}

 */