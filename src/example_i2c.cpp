/*
void setup() {
     ....
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
     ...
 }


void loop() {
...
if (button.pressed()) {
     ...
     Wire.beginTransmission(0x74); // Set all LOW
     Wire.write(0x03);  // Output Port 1
     Wire.write(0xFF);  // Set all LOW
     Wire.endTransmission(); // Set all HIGH
     delay(1000);

     Wire.beginTransmission(0x74); // Set all HIGH
     Wire.write(0x03);  // Output Port 1
     Wire.write(0x00);  // Set all LOW
     Wire.endTransmission(); // Set all HIGH
     delay(1000);
     ...
     }
}
*/