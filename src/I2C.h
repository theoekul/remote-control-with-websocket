#include <Wire.h>
#include <Arduino.h>

#define TCA9539_ADDRESS 0x74  // I2C address for TCA9539PWR (can be different based on your wiring)
#define INPUT_PORT 0x00       // Input port register address
#define OUTPUT_PORT 0x01      // Output port register address
#define CONFIG_PORT 0x03      // Configuration register address


void initPinDirection(){
Wire.beginTransmission(TCA9539_ADDRESS);
  Wire.write(CONFIG_PORT); // Access the configuration register
  Wire.write("direction");   // 0xFF for all pins as output, 0x00 for all pins as input
  Wire.endTransmission();
  
  }
  
  

void writeOutputPort(uint8_t value) {
  Wire.beginTransmission(TCA9539_ADDRESS);
  Wire.write(OUTPUT_PORT); // Access the output register
  Wire.write(value);       // Value to output (0xFF = all high, 0x00 = all low)
  Wire.endTransmission();

  }
// Function to read the input port
uint8_t readInputPort() {
  Wire.beginTransmission(TCA9539_ADDRESS);
  Wire.write(INPUT_PORT);  // Access the input register
  Wire.endTransmission();
  
  Wire.requestFrom(TCA9539_ADDRESS, 1); // Request 1 byte from the input register
  if (Wire.available()) {
    return Wire.read();  // Return the byte read from the input port
  }
  return 0;
}
