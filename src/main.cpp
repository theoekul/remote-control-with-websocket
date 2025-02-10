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
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "secrets.h"
#include "rotationread.h" // Include header file for rotary switch
#include "swr_led.h" // Include header file for SWR display
#include "I2C.h"

// ----------------------------------------------------------------------------
// Definition of macros
// ----------------------------------------------------------------------------
#define I2C_EXPANDER_ADDR 39// I2C expander address sdl
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
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
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
    JsonDocument json;
    json["status"] = led.on ? "on" : "off";
    json["status_vu"] = analogRead(A0);

    char buffer[200];
    size_t len = serializeJson(json, buffer);
    ws.textAll(buffer, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) { // This function handles messages from the WebSocket
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

        JsonDocument json;
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

void initWebSocket() { // This function initializes the WebSocket
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void initStrip() { // This function initializes the NeoPixel strip
    strip.begin();
    strip.show();
}

// ----------------------------------------------------------------------------
// I2C Expander initialization
// ----------------------------------------------------------------------------

void initI2CExpander() {
    Wire.begin(21,47); // Initialize I2C communication
    unsigned long startTime = millis();
    bool initialized = false;
    while (millis() - startTime < 5000) { // 5 seconds timeout
        Wire.beginTransmission(I2C_EXPANDER_ADDR);
        if (Wire.endTransmission() == 0) {
            Serial.println("I2C Expander initialized successfully.");
            initialized = true;
            break;
        }
    }
    if (!initialized) {
        Serial.println("Failed to initialize I2C Expander.");
    }
}


// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

void setup() { // This function initializes the program and sets up the components
    pinMode(led.pin,         OUTPUT);
    pinMode(button.pin,      INPUT);
    pinMode(NEO_PIN,         OUTPUT);

    Serial.begin(115200);
    //Serial.begin(9600);

    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
    initSWRDisplay();
    initRotRead();
    initStrip();
    strip.setPixelColor(0, 0, 50, 0);
    strip.show();
    // Initialize the I2C communication (specify SDA and SCL pins for ESP32)
    Wire.begin(21, 22);  // GPIO21 for SDA, GPIO22 for SCL on ESP32
    // Initialize Serial monitor for debugging
    Serial.begin(115200);
    // Set all pins to output by default (this is for demonstration)
    // You can change the configuration depending on your needs.
    //initPinDirection(0xFF);  // Set all pins as output (0xFF = all 1s)
    delay(1000);  // Wait for configuration
    // Set all pins to high (turn on)
    //writeOutputPort(0xFF);
    delay(1000);  // Wait for 1 second
    // Set all pins to low (turn off)
    writeOutputPort(0x00);
}

// ----------------------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------------------

unsigned long previousMillis = 0;
uint16_t previousAnalog = 0;

void loop() { // This function is the main loop of the program and is responsible for reading the button, rotary switch and updating the LED
    ws.cleanupClients();
    
    button.read();

    readRotarySwitch();
    delay(500); // Adjust the delay as needed
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
    
    if ((meas > previousAnalog + 30) || (meas < previousAnalog - 30)) { //Denne delen av koden er for å oppdatere SWR displayet
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= 200) {
            // save the last time you blinked the LED
            previousMillis = currentMillis;
            
            notifyClients();
            previousAnalog = meas;
        }
    }
    // Read the current state of the input pins (if used as input)
      
    delay(500);  // Delay for half a second
    uint8_t inputState = readInputPort();
    Serial.print("Input State: ");
    Serial.println(inputState, BIN);
    strip.show();
    led.update();
}
