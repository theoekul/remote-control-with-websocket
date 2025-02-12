#pragma once
#ifndef TCA9539_H
#define TCA9539_H

#include <Arduino.h>
#include <Wire.h>

namespace arduino
{
    template <typename WireType, uint8_t N>
    class IoEx8bit
    {
        uint8_t I2C_ADDR = 0x74;

        WireType* wire;
        uint8_t sts;

    public:

        enum class Port { PORT1 = 0, PORT2 = 1 };
        enum class Reg { INPUT_PORT1, INPUT_PORT2, OUTPUT_PORT1, OUTPUT_PORT2, POLARITY1, POLARITY2, CONFIG1, CONFIG2 };
        enum class Config { OUT, IN };
        enum class Polarity { ORIGINAL, INVERSE };
        enum Level { L = 0x00, H = 0xFF };

        void attach(WireType& w) { wire = &w; }

        void setDeviceAddress(const uint8_t addr) { I2C_ADDR = addr; }

        uint8_t input(const Port port, const uint8_t pin) {
            if(port == Port::PORT1){
                return readBit(I2C_ADDR, (uint8_t)Reg::INPUT_PORT1, pin);
            } else if (port == Port::PORT2){
                return readBit(I2C_ADDR, (uint8_t)Reg::INPUT_PORT2, pin);
            } else {
                return -1;
            }
        }
        uint8_t input(const Port port) {
            if(port == Port::PORT1){
                return readByte(I2C_ADDR, (uint8_t)Reg::INPUT_PORT1);
            } else if (port == Port::PORT2){
                return readByte(I2C_ADDR, (uint8_t)Reg::INPUT_PORT2);
            } else {
                return -1;
            }
        }

        uint8_t output(const Port port, const uint8_t pin, const uint8_t state) {
            if(port == Port::PORT1){
                return writeBit(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT1, pin, (uint8_t)(state & 0x01));
            } else if (port == Port::PORT2){
                return writeBit(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT2, pin, (uint8_t)(state & 0x01));   
            } else {
                return -1;
            }
        }
        uint8_t output(const Port port, const uint8_t state) {
            if(port == Port::PORT1){
                return writeByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT1, state);
            } else if (port == Port::PORT2){
                return writeByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT2, state);
            } else {
                return -1;
            }
        }
        uint8_t output(const Port port) {
            if(port == Port::PORT1){
                return readByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT1);
            } else if (port == Port::PORT2){
                return readByte(I2C_ADDR, (uint8_t)Reg::OUTPUT_PORT2);
            } else {
                return -1;
            }
        }

        uint8_t polarity(const Port port, const uint8_t pin, const Polarity pol)
        {
            if(port == Port::PORT1){
                return writeBit(I2C_ADDR, (uint8_t)Reg::POLARITY1, pin, (uint8_t)pol);
            } else if (port == Port::PORT2){
                return writeBit(I2C_ADDR, (uint8_t)Reg::POLARITY2, pin, (uint8_t)pol);
            } else {
                return -1;
            }
        }
        uint8_t polarity(const Port port, const Polarity pol) {
            uint8_t d = (pol == Polarity::ORIGINAL) ? 0x00 : 0xFF;
            if(port == Port::PORT1){
                return writeByte(I2C_ADDR, (uint8_t)Reg::POLARITY1, (uint8_t)d);
            } else if (port == Port::PORT2){
                return writeByte(I2C_ADDR, (uint8_t)Reg::POLARITY2, (uint8_t)d);
            } else {
                return -1;
            }
        }
        uint8_t polarity(const Port port) {
            if(port == Port::PORT1){
                return readByte(I2C_ADDR, (uint8_t)Reg::POLARITY1);
            } else if (port == Port::PORT2){
                return readByte(I2C_ADDR, (uint8_t)Reg::POLARITY2);
            } else {
                return -1;
            }
        }

        uint8_t config(const Port port, const uint8_t pin, const Config cfg) {
            if(port == Port::PORT1){
                return writeBit(I2C_ADDR, (uint8_t)Reg::CONFIG1, pin, (uint8_t)cfg);
            } else if (port == Port::PORT2){
                return writeBit(I2C_ADDR, (uint8_t)Reg::CONFIG2, pin, (uint8_t)cfg);
            } else {
                return -1;
            }
        }
        uint8_t config(const Port port, const Config cfg) {
            uint8_t d = (cfg == Config::OUT) ? 0x00 : 0xFF;
            if(port == Port::PORT1){
                return writeByte(I2C_ADDR, (uint8_t)Reg::CONFIG1, (uint8_t)d);
            } else if (port == Port::PORT2){
                return writeByte(I2C_ADDR, (uint8_t)Reg::CONFIG2, (uint8_t)d);
            } else {
                return -1;
            }
        }
        uint8_t config(const Port port) {
            if(port == Port::PORT1){
                return readByte(I2C_ADDR, (uint8_t)Reg::CONFIG1);
            } else if (port == Port::PORT2){
                return readByte(I2C_ADDR, (uint8_t)Reg::CONFIG2);
            } else {
                return -1;
            }
        }

        uint8_t status() const { return sts; }

    private:

        uint8_t readBit(uint8_t dev, uint8_t reg, uint8_t bit)
        {
            uint8_t b = readByte(dev, reg);
            b &= (1 << bit);
            return b;
        }

        uint8_t readByte(uint8_t dev, uint8_t reg)
        {
            uint8_t data;
            readBytes(dev, reg, 1, &data);
            return data;
        }

        int8_t readBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t *data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(dev, size);
            int8_t count = 0;
            while (wire->available()) data[count++] = wire->read();
            return count;
        }

        bool writeBit(uint8_t dev, uint8_t reg, uint8_t bit, uint8_t data)
        {
            uint8_t b = readByte(dev, reg);
            b = (data != 0) ? (b | (1 << bit)) : (b & ~(1 << bit));
            return writeByte(dev, reg, b);
        }

        bool writeByte(uint8_t dev, uint8_t reg, uint8_t data)
        {
            return writeBytes(dev, reg, 1, &data);
        }

        bool writeBytes(uint8_t dev, uint8_t reg, uint8_t size, uint8_t* data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            for (uint8_t i = 0; i < size; i++)
                wire->write(data[i]);
            sts = wire->endTransmission();
            if (sts != 0)
            {
                Serial.print("I2C ERROR : ");
                Serial.println(sts);
            }
            return (sts == 0);
        }

    };

}

using TCA9539 = arduino::IoEx8bit<TwoWire, 8>;

#endif // TCA9539_H_
