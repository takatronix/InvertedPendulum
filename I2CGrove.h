#pragma once
#include <Wire.h>

#define I2CGROVE_DEFAULT_FREQUENCY 400000
class I2CGrove
{
public:
    uint8_t _addr;
    TwoWire *_wire;

    bool begin(uint16_t addr)
    {
        LOG_D("I2CGrove begin: addr=%xh, frequency=%d", addr);
        _wire = &Wire;
        _addr = addr;
        _wire->begin();
        delay(10);
        _wire->beginTransmission(_addr);
        uint8_t error = _wire->endTransmission();

        if (error == 0) {
            LOG_D("I2CGrove begin: success");
            return true;
        } 
        
        LOG_E("I2CGrove begin: error=%d", error);
        return false; 
    }

    void end()
    {
        _wire->end();
    }
    
    void writeByte(uint8_t reg, uint8_t data)
    {
        LOG_D("writeByte: address=%x, reg=%d, data=%d", _addr, reg, data);
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        _wire->write(data);
        _wire->endTransmission();
    }

    uint8_t readByte(uint8_t reg)
    {
        uint8_t data;
        readBytes(reg, &data, 1);
        return data;
    }

    // big endian
    uint16_t readWord(uint8_t reg)
    {
        uint8_t data[2];
        readBytes(reg, data, 2);
        return (data[1] << 8) | data[0];
    }

    int writeBytes(uint8_t reg, uint8_t *data, uint8_t length)
    {
        //LOG_D("writeBytes: address=%x, reg=%d, length=%d", _addr, reg, length);
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        int result = 0;
        for (uint8_t i = 0; i < length; i++)
        {
            auto ret = _wire->write(data[i]);
            result += ret;
        }
        _wire->endTransmission();
        //LOG_D("writeBytes: result=%d", result);
        return result;
    }

    bool readBytes(uint8_t reg, uint8_t *buffer,uint8_t length) {
        //LOG_D("readBytes: address=%x, reg=%d, length=%d", _addr, reg, length);
        uint8_t index = 0;
        _wire->beginTransmission(_addr);
        _wire->write(reg);
        _wire->endTransmission();
        if (_wire->requestFrom(_addr, length)) {
            for (uint8_t i = 0; i < length; i++) {
                buffer[index++] = _wire->read();
            }
            return true;
        }
        LOG_W("readBytes failed: address=%x, reg=%d, length=%d", _addr, reg, length);
        return false;
    }

};
