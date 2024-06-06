#pragma once

//////////////////////////////////////////////
//    Configuration Settings
//////////////////////////////////////////////
#define MOTOR_I2C_WIRE Wire
#define MOTOR_I2C_ADDR HAT_C_BACK_I2C_ADDR
#define MOTOR_I2C_SDA HAT_C_BACK_SDA
#define MOTOR_I2C_SCL HAT_C_BACK_SCL


#define ANGLE_USE_ROLL false // Set to true if Atom intersects with the axis
#define TUMBLE_DETECT true  // Set to true to enable tumble detection
// Tumble detection angles (only valid if TUMBLE_DETECT is true)
#define PITCH_MIN -60.0f
#define PITCH_MAX 60.0f
#define ROLL_MIN -60.0f
#define ROLL_MAX 60.0f

// 1. Motor and Servo Settings
// Set USE_SERVO to true if using a 360-degree free rotation servo
#define REVERSE false   // Set to true to reverse the rotation direction
#define USE_SERVO true // Set to true to use a servo
// Servo channel numbers
#define SERVO_CH_LEFT 0  // Left servo channel
#define SERVO_CH_RIGHT 1 // Right servo channel
// Set USE_MOTOR to true if using DC motors
#define USE_MOTOR false   // Set to true to use a motor
#define MOTOR_CH_LEFT 0  // Left motor channel
#define MOTOR_CH_RIGHT 1 // Right motor channel

// 2. Target Angle (important, generally set to 0)
// Set the value to the angle at which balance is achieved, or
// set to 0, power off, balance the system, and then power on to calibrate
// The target angle is the angle at which the system can stand upright when powered off
// #define TARGET_ANGLE          -21.47f
#define TARGET_ANGLE 0.0f
#define CALIBRATE_ON_SETUP false // Set to true to perform calibration

// 3. PID Control Settings (important, difficult to adjust)
// (1) First set KI and KD to 0, increase KP value to achieve balance
// (2) Next, increase KD value to minimize oscillations
// (3) Finally, adjust KI value to achieve balance
#define KP 20 // Proportional gain, increase to speed up response
#define KI 0.42 // Integral gain, set low
#define KD 1 // Derivative gain, set appropriately

#define SAMPLE_PERIOD 0.005 // Sampling period (seconds)

// Extended Kalman Filter Settings (generally no need to change)
// Self-position estimation settings
#define EKF_ENABLE true // Enable extended Kalman filter
// Example settings
// For prioritizing speed: 0.01f, 0.003f, 0.03f
// For prioritizing quality: 0.001f, 0.0003f, 0.1f

// Process noise covariance matrix angle element
// Indicates how much uncertainty the system has regarding the angle
#define EKF_Q_ANGLE 0.02f
// Process noise covariance matrix bias element
// Indicates how much uncertainty the system has regarding the bias
#define EKF_Q_BIAS 0.006f
// Observation noise covariance matrix angle element
// Indicates how much noise is present in the measurements
// If the value is 0, it assumes there is no noise (or it is negligible)
#define EKF_R_MEASURE 0.03f
