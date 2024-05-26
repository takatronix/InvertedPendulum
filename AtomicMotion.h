#pragma once

#define SERVO_PULSE_MIN 500
#define SERVO_PULSE_CENTER 1500
#define SERVO_PULSE_MAX 2500
#define SERVO_ANGLE_MIN 0
#define SERVO_ANGLE_CENTER 90
#define SERVO_ANGLE_MAX 180
#define MOTOR_SPEED_MIN -127
#define MOTOR_SPEED_CENTER 0
#define MOTOR_SPEED_MAX 127

using namespace m5;
class AtomicMotion
{
    I2C_Device *i2c;

public:
    static const uint8_t DEFAULT_ADDRESS = 0x38;
    static const uint32_t DEFAULT_FREQ = 400000;
    AtomicMotion(uint8_t i2c_addr = DEFAULT_ADDRESS, uint32_t freq = DEFAULT_FREQ)
    {
        i2c = new I2C_Device(i2c_addr, freq);
        stopAllMotors();
    }

    bool writeRegister16(uint8_t reg, uint16_t data)
    {
        uint8_t buf[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
        return i2c->writeRegister(reg, buf, sizeof(buf));
    }

    uint16_t readRegister16(uint8_t reg)
    {
        uint8_t buf[2];
        i2c->readRegister(reg, buf, sizeof(buf));
        uint16_t result = (((uint16_t)buf[0]) << 8) | buf[1];
        return result;
    }

    // angle: 0-180
    uint8_t setServoAngle(uint8_t ch, uint8_t angle)
    {
        if (ch > 3)
            return 0;
        i2c->writeRegister8(ch, (uint8_t)angle);
        return 0;
    }

    // pulse: 500-2500
    uint8_t setServoPulse(uint8_t ch, uint16_t width)
    {
        uint8_t adr = 2 * ch + 16;
        if (adr % 2 == 1 || adr > 32)
            return 1;
        this->writeRegister16(adr, width);
        return 0;
    }

    // speed: -127-127
    uint8_t setMotorSpeed(uint8_t ch, int8_t speed)
    {
        if (ch > 1)
            return 0;
        uint8_t adr = ch + 32;
        i2c->writeRegister8(adr, (uint8_t)speed);
        return 0;
    }

    uint8_t getServoAngle(uint8_t ch)
    {
        return i2c->readRegister8(ch);
    }

    uint16_t getServoPulse(uint8_t ch)
    {
        uint8_t adr = 2 * ch | 0x10;
        return this->readRegister16(adr);
    }

    int8_t getMotorSpeed(uint8_t ch)
    {
        int8_t data = 0;
        if (ch > 1)
            return -1;
        uint8_t adr = ch | 0x20;
        return i2c->readRegister8(adr);
    }

    // speed -127 - 127を、500 - 2500のパルス幅に変換
    uint8_t setServoSpeed(uint8_t ch, int speed)
    {
        if (ch > 3)
            return 1;
        if (speed < MOTOR_SPEED_MIN)
            speed = MOTOR_SPEED_MIN;
        if (speed > MOTOR_SPEED_MAX)
            speed = MOTOR_SPEED_MAX;
        int pulse = map(speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX, SERVO_PULSE_MIN, SERVO_PULSE_MAX);

        return setServoPulse(ch, pulse);
    }

    void stopAllMotors()
    {
        // stop all motors
        setMotorSpeed(0, MOTOR_SPEED_CENTER);
        setMotorSpeed(1, MOTOR_SPEED_CENTER);
        // stop all servos
        setServoPulse(0, SERVO_PULSE_CENTER);
        setServoPulse(1, SERVO_PULSE_CENTER);
        setServoPulse(2, SERVO_PULSE_CENTER);
        setServoPulse(3, SERVO_PULSE_CENTER);
    }
};
