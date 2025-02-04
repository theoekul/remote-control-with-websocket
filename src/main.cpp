/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"
#include "rotationread.h"
#include "swr_led.h"

// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------

#define LED_PIN   4
#define BTN_PIN   0
#define NEO_PIN   38
#define NEO_COUNT 1
#define HTTP_PORT 80

// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------

// Button debouncing
const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds


// WiFi credentials
const char *WIFI_SSID = ssid_name;
const char *WIFI_PASS = ssid_password;

// ----------------------------------------------------------------------------
// Definition of the LED component
// ----------------------------------------------------------------------------

struct Led {
    // state variables
    uint8_t pin;
    bool    on;

    // methods
    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
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
                     if (state  < 0xfffe) state++;
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

Adafruit_NeoPixel strip(NEO_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);

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

void initWiFi() { //Denne funksjonen kobler til wifi nettverket
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// ----------------------------------------------------------------------------
// Web server initialization
// ----------------------------------------------------------------------------

String processor(const String &var) { //Denne funksjonen prosesserer variabler som skal sendes til klienten
    return String(var == "STATE" && led.on ? "on" : "off");
}

void onRootRequest(AsyncWebServerRequest *request) { //Denne funksjonen sender index.html til klienten
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer() { //Denne funksjonen setter opp webserveren
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients() { //Denne funksjonen sender meldinger til alle klienter som er koblet til websocketen
    JsonDocument json;
    json["status"] = led.on ? "on" : "off";
    json["status_vu"] = analogRead(A0);

    char buffer[200];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) { //Denne funksjonen håndterer meldinger som kommer fra websocketen
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        JsonDocument json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }

        const char *action = json["action"];
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

void onEvent(AsyncWebSocket       *server,
             AsyncWebSocketClient *client,
             AwsEventType          type,
             void                 *arg,
             uint8_t              *data,
             size_t                len) {

    switch (type) {
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

void initStrip() {
    strip.begin();
    strip.show();
}
// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() {
    pinMode(led.pin,         OUTPUT);
    pinMode(button.pin,      INPUT);
    pinMode(NEO_PIN,         OUTPUT);

    Serial.begin(115200); delay(500);

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
    // initRotarySwitch(); // Function not defined, commented out
    initSWRDisplay();
    initRotRead();
    initRotarySwitch();
    initStrip();
    strip.setPixelColor(0, 0, 50, 0);
    strip.show();
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

unsigned long previousMillis = 0;
uint16_t previousAnalog = 0;

void loop() {
    ws.cleanupClients();

    button.read();

    if (button.pressed()) {
        Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
        led.on = !led.on;
        if (led.on) {
            strip.setPixelColor(0, 100, 0, 0);
        } else {
            strip.setPixelColor(0, 0, 0, 0);
        }        
        notifyClients();
    }
    uint16_t meas = analogRead(A0);
    
    if ((meas > previousAnalog + 30) || (meas < previousAnalog - 30)) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 200) {
            // save the last time you blinked the LED
            previousMillis = currentMillis;
            
            notifyClients();
            previousAnalog = meas;
        }
    }
    strip.show();
    led.update();
}