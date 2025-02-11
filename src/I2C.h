/*
#ifndef I2C_H
#define I2C_H

#include <Wire.h>
#include <Arduino.h>

#define TCA9539_ADDRESS 0x74  // I2C address for TCA9539PWR (can be different based on your wiring)
#define INPUT_PORT 0x00       // Input port register address
#define OUTPUT_PORT 0x01      // Output port register address
#define CONFIG_PORT 0x03      // Configuration register address

void initPinDirection(uint8_t direction) {
    Wire.beginTransmission(TCA9539_ADDRESS);
    Wire.write(CONFIG_PORT); // Access the configuration register
    Wire.write(direction);   // 0xFF for all pins as output, 0x00 for all pins as input
    Wire.endTransmission();
}

void writeOutputPort(uint8_t value) {
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { // 1 second timeout
        Wire.beginTransmission(TCA9539_ADDRESS);
        Wire.write(OUTPUT_PORT); // Access the output register
        Wire.write(value);       // Value to output (0xFF = all high, 0x00 = all low)
        if (Wire.endTransmission() == 0) {
            return; // Success
        }
    }
    Serial.println("Failed to write to I2C Expander.");
}

uint8_t readInputPort() {
    unsigned long startTime = millis();
    while (millis() - startTime < 1000) { // 1 second timeout
        Wire.beginTransmission(TCA9539_ADDRESS);
        Wire.write(INPUT_PORT);  // Access the input register
        if (Wire.endTransmission() == 0) {
            Wire.requestFrom(TCA9539_ADDRESS, 1); // Request 1 byte from the input register
            if (Wire.available()) {
                return Wire.read();  // Return the byte read from the input port
            }
        }
    }
    Serial.println("Failed to read from I2C Expander.");
    return 0;
}

#endif // I2C_H
*/