#pragma once
#include "I2CWire.h"

#define SERVO_PULSE_MIN 500
#define SERVO_PULSE_CENTER 1500
#define SERVO_PULSE_MAX 2500
#define SERVO_ANGLE_MIN 0
#define SERVO_ANGLE_CENTER 90
#define SERVO_ANGLE_MAX 180
#define MOTOR_SPEED_MIN -127
#define MOTOR_SPEED_CENTER 0
#define MOTOR_SPEED_MAX 127

#define ATOM_MOTION_I2C_ADDR 0x38
#define ATOM_MOTION_SDA 25
#define ATOM_MOTION_SCL 21
#define ATOM_MOTION_MAX_SERVOS 4
#define ATOM_MOTION_MAX_MOTORS 2

#define HAT_C_BACK_I2C_ADDR 0x38
#define HAT_C_BACK_SDA 0
#define HAT_C_BACK_SCL 26
#define HAT_C_BACK_MAX_SERVOS 4

#define HAT_8SERVOS_v1_1_I2C_ADDR 0x36
#define HAT_8SERVOS_v1_1_SDA 0
#define HAT_8SERVOS_v1_1_SCL 26
#define HAT_8SERVOS_v1_1_MAX_SERVOS 8

/*
    Atom Motion Spec
    I2C adr: 0x38
    Servo(1-4)          Reg 0x00-0x03               Range:0-180     R/W
    Servo Pulse(1-4)    Reg 0x10,0x12,0x14,0x16     Range:500-2500  R/W
    Motor(1-2)          Reg 0x20,0x21               Range:-127-127  R/W
    https://docs.m5stack.com/ja/atom/atom_motion
*/

/*  C Back Driver Spec
    I2C adr: 0x38
    Servo(1-4)          Reg 0x00-0x03               Range:0-180     R/W
    Servo Pulse(1-4)    Reg 0x10,0x12,0x14,0x16     Range:500-2500  R/W
    https://docs.m5stack.com/ja/hat/c_back_driver
*/

/*  8 Servos HAT v1.1 Spec
    I2C adr: 0x36
    Servo(1-8)          Reg 0x00-0x07               Range:0-180     R/W
    Servo Pulse(1-8)    Reg 0x10,0x12,...0x30       Range:500-2500  R/W
    https://docs.m5stack.com/ja/hat/hat_8servos_1.1
*/
#define DEFAULT_adr 0x38
#define DEFAULT_SDA 0
#define DEFAULT_SCL 26

using namespace m5;
class MotorDriver : public I2CWire
{
public:

    bool begin(TwoWire& wire, uint16_t addr, int sda = -1, int scl = -1)
    {
        if (I2CWire::begin(wire, addr, sda, scl))
        {
            return 1;
        }
        return 0;
    }

    // DCモーター(M5 Motion)のみ speed: -127-127
    uint8_t setMotorSpeed(uint8_t ch, int8_t speed)
    {
        ch--;
        uint8_t adr = ch + 0x20;
        writeByte(adr, (uint8_t)speed);
        return 0;
    }

    // speed -127 - 127を、500 - 2500のパルス幅に変換
    uint8_t setServoSpeed(uint8_t ch, int speed)
    {
        LOG_D("setServoSpeed: ch=%d, speed=%d", ch, speed);
        if (speed < MOTOR_SPEED_MIN)
            speed = MOTOR_SPEED_MIN;
        if (speed > MOTOR_SPEED_MAX)
            speed = MOTOR_SPEED_MAX;
        int pulse = map(speed, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX, SERVO_PULSE_MIN, SERVO_PULSE_MAX);

        return setServoPulse(ch, pulse);
    }

    uint8_t setServoAngle(uint8_t ch, uint8_t angle)
    {
        uint8_t reg = ch - 1;
        writeByte(reg, angle);
        return 0;
    }

    uint8_t setServoPulse(uint8_t ch, uint16_t width)
    {
        ch--;
        uint8_t reg = 2 * ch + 16;
        if (reg % 2 == 1 || reg > 32)
            return 1;
        writeWord(reg, width);
        return 0;
    }

    uint8_t getServoAngle(uint8_t ch)
    {
        uint8_t reg = ch - 1;
        uint8_t data = 0;
        return readByte(reg);
    }

    uint16_t getServoPulse(uint8_t ch)
    {
        ch--;
        uint8_t reg = 2 * ch | 0x10;
        return readWord(reg);
    }
};

class AtomMotion : public MotorDriver
{
public:
    bool begin()
    {
        return MotorDriver::begin(Wire, ATOM_MOTION_I2C_ADDR, ATOM_MOTION_SDA, ATOM_MOTION_SCL);
    }
};

class HatCBack : public MotorDriver
{
public:
    bool begin()
    {
        return MotorDriver::begin(Wire, HAT_C_BACK_I2C_ADDR, HAT_C_BACK_SDA, HAT_C_BACK_SCL);
    }
};
