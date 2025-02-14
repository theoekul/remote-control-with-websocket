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
#include <Wire.h>
#include "secrets.h"
#include "rotswitch.h"
#include "swr_led.h"
#include "tca9539.h"


// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------

#define LED_PIN   4
#define BTN_PIN   0
#define NEO_PIN   38
#define NEO_COUNT 1
#define HTTP_PORT 80
#define SCL_PIN   47
#define SDA_PIN   21
#define IO_EXP_1_ADDR 0x74

// ----------------------------------------------------------------------------
// Definition of global constants
// ----------------------------------------------------------------------------


uint8_t wanted_dir = 0;
uint8_t actual_dir = 0;
uint8_t lastRotaryDir = 0;

unsigned long lastNotifyClientMillis;

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

TCA9539 ioex1;

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

void initWiFi() {
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

String processor(const String &var) {
    return String(var == "STATE" && led.on ? "on" : "off");
}

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer() {
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// ----------------------------------------------------------------------------
// WebSocket initialization
// ----------------------------------------------------------------------------

void notifyClients() {
    JsonDocument json;
    json["status"] = led.on ? "on" : "off";
    json["dir"] = actual_dir;

    char buffer[300];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
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
    initStrip();
    initRotarySwitch();
    lastRotaryDir = readRotarySwitch();
    
    //initSWRLEDs();
    strip.setPixelColor(0, 0, 50, 0);
    strip.show();

    Serial.printf("Init I2C");
    Wire.begin(SDA_PIN, SCL_PIN);
    ioex1.attach(Wire);
    ioex1.setDeviceAddress(IO_EXP_1_ADDR);
    ioex1.config(TCA9539::Port::PORT1, TCA9539::Config::OUT);
    ioex1.config(TCA9539::Port::PORT2, TCA9539::Config::OUT);
    Serial.printf("Init I2C DONE!");
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

void loop() {
    ws.cleanupClients();

    // Send state update to web clients once every second
    if (millis() - lastNotifyClientMillis >= 1000UL) 
    {
        lastNotifyClientMillis = millis();  //get ready for the next iteration
        notifyClients();
    }


   
    // Check rotary switch for changes
    uint8_t newDir = readRotarySwitch();
    if (lastRotaryDir != newDir) {
        // Setting of rotary switch changed
        // Update wanted direction
        lastRotaryDir = newDir;
        wanted_dir = newDir;
    }
    

    
    // check if a new direction is wanted:
    if (actual_dir != wanted_dir)
    {
        // Check here if it is safe to switch direction, always true for now
        if (true)
        {
            /* switch the actual relays here */
            actual_dir = wanted_dir; // update the actual direction variable
            notifyClients();
        }
    }



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
        /*
        // Set all pins(8) on entire port:
        ioex1.output(TCA9539::Port::PORT2, 0xFF);
        delay(1000);
        ioex1.output(TCA9539::Port::PORT2, 0x00);
        delay(1000);

        // Set one pin only:
        ioex1.output(TCA9539::Port::PORT2, 2, 1);
        delay(1000);
        ioex1.output(TCA9539::Port::PORT2, 2, 0);
        delay(1000);
        */
    }
    strip.show();
    led.update();
}
