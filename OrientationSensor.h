#pragma once
#include <M5Unified.h>
#include <math.h>

// EKF(拡張カルマンフィルタ)クラス
class EKF
{
public:
    EKF()
    {
        Q_angle = 0.001f;
        Q_bias = 0.003f;
        R_measure = 0.03f;
        angle = 0.0f;
        bias = 0.0f;
        rate = 0.0f;
        P[0][0] = 0;
        P[0][1] = 0;
        P[1][0] = 0;
        P[1][1] = 0;
    }

    float getAngle(float newAngle, float newRate, float dt)
    {
        // 予測ステップ
        rate = newRate - bias;
        angle += dt * rate;

        // 予測誤差共分散の更新
        P[0][0] += dt * (dt * P[1][1] - P[0][1] - P[1][0] + Q_angle);
        P[0][1] -= dt * P[1][1];
        P[1][0] -= dt * P[1][1];
        P[1][1] += Q_bias * dt;

        // 更新ステップ
        float S = P[0][0] + R_measure;
        float K[2];
        K[0] = P[0][0] / S;
        K[1] = P[1][0] / S;

        float y = newAngle - angle;
        angle += K[0] * y;
        bias += K[1] * y;

        float P00_temp = P[0][0];
        float P01_temp = P[0][1];

        P[0][0] -= K[0] * P00_temp;
        P[0][1] -= K[0] * P01_temp;
        P[1][0] -= K[1] * P00_temp;
        P[1][1] -= K[1] * P01_temp;

        return angle;
    }

    void reset()
    {
        angle = 0.0f;
        bias = 0.0f;
        P[0][0] = 0;
        P[0][1] = 0;
        P[1][0] = 0;
        P[1][1] = 0;
    }

    void setNoiseParameters(float q_angle, float q_bias, float r_measure)
    {
        Q_angle = q_angle;
        Q_bias = q_bias;
        R_measure = r_measure;
    }

private:
    float Q_angle;
    float Q_bias;
    float R_measure;

    float angle;
    float bias;
    float rate;

    float P[2][2];
};

// 姿勢推定クラス
// ジャイロセンサーと加速度センサーを用いて姿勢を推定する
class OrientationSensor
{
public:
    OrientationSensor() : pitchEKF(), rollEKF(), yawEKF(), accXEKF(), accYEKF(), accZEKF(),
                          pitchOffset(0.0f), rollOffset(0.0f), yawOffset(0.0f),
                          accX(0.0f), accY(0.0f), accZ(0.0f),
                          gyroX(0.0f), gyroY(0.0f), gyroZ(0.0f),
                          pitch(0.0f), roll(0.0f), yaw(0.0f),
                          samplePeriod(0.01f) {}

    bool disableGyro = false;
    void calibrate()
    {
        float tempAccX, tempAccY, tempAccZ;
        float tempGyroX, tempGyroY, tempGyroZ;

        // 初期オフセットを計算するためにセンサーのデータを数回平均化
        const int calibrationSamples = 100;
        for (int i = 0; i < calibrationSamples; ++i)
        {
            M5.Imu.getAccelData(&tempAccX, &tempAccY, &tempAccZ);
            M5.Imu.getGyroData(&tempGyroX, &tempGyroY, &tempGyroZ);
            pitchOffset += atan2(tempAccY, tempAccZ) * RAD_TO_DEG;
            rollOffset += atan2(-tempAccX, tempAccZ) * RAD_TO_DEG;
            gyroXOffset += tempGyroX;
            gyroYOffset += tempGyroY;
            gyroZOffset += tempGyroZ;
            delay(10); // サンプル間の遅延
        }
        pitchOffset /= calibrationSamples;
        rollOffset /= calibrationSamples;
        gyroXOffset /= calibrationSamples;
        gyroYOffset /= calibrationSamples;
        gyroZOffset /= calibrationSamples;

        // 初期位置の角度をEKFにセット
        setNoiseParameters(this->_q_angle, this->_q_bias, this->_r_measure);
        LOG_D("Calibrated: Pitch:%.2f Roll:%.2f GyroX:%.2f GyroY:%.2f GyroZ:%.2f\n", pitchOffset, rollOffset, gyroXOffset, gyroYOffset, gyroZOffset);
    }

    void update()
    {
        // Imuセンサーからデータを読み取る
        M5.Imu.getAccelData(&accX, &accY, &accZ);
        M5.Imu.getGyroData(&gyroX, &gyroY, &gyroZ);

        // ジャイロのオフセットを補正
        gyroX -= gyroXOffset;
        gyroY -= gyroYOffset;
        gyroZ -= gyroZOffset;
        if (disableGyro)
        {
            gyroX = 0;
            gyroY = 0;
            gyroZ = 0;
        }
        // サンプリング周期を計算
        float dt = samplePeriod;

        // 加速度データからピッチとロールを計算
        accPitch = atan2(accY, accZ) * RAD_TO_DEG;
        accRoll = atan2(-accX, accZ) * RAD_TO_DEG;

        // カルマンフィルタを用いてピッチとロールを推定
        pitch = pitchEKF.getAngle(accPitch - pitchOffset, gyroX, dt);
        roll = rollEKF.getAngle(accRoll - rollOffset, gyroY, dt);

        // ヨー角の推定（ジャイロデータのみ）
        yaw += gyroZ * dt;
        yaw = fmod(yaw + 360.0f, 360.0f); // 0度から360度の範囲に制限

        // 加速度データのEKF処理
        accXFiltered = accXEKF.getAngle(accX, gyroX, dt);
        accYFiltered = accYEKF.getAngle(accY, gyroY, dt);
        accZFiltered = accZEKF.getAngle(accZ, gyroZ, dt);
    }

    float getPitch(bool ekfEnabled = true) const
    {
        if(!ekfEnabled)
            return accPitch;
        return pitch;
    }

    float getRoll(bool ekfEnabled = true) const
    {
        if(!ekfEnabled)
            return accRoll;
        return roll;
    }

    float getYaw() const
    {
        return yaw;
    }
    float getPitchOffset() const
    {
        return pitchOffset;
    }
    float getRollOffset() const
    {
        return rollOffset;
    }
    float getYawOffset() const
    {
        return yawOffset;
    }
    void getCalibratedAcceleration(float &x, float &y, float &z) const
    {
        // 姿勢角度に基づいて加速度データを補正
        x = accXFiltered;
        y = accYFiltered;
        z = accZFiltered;
    }

    void reset()
    {
        pitchEKF.reset();
        rollEKF.reset();
        yawEKF.reset();
        accXEKF.reset();
        accYEKF.reset();
        accZEKF.reset();
        yaw = 0.0f;
    }

    void setSamplePeriod(float period)
    {
        samplePeriod = period;
    }

    void setSampleRate(float rate)
    {
        if (rate > 0)
        {
            samplePeriod = 1.0f / rate;
        }
    }

    float _q_angle = 0.001f;
    float _q_bias = 0.003f;
    float _r_measure = 0.03f;
    void setNoiseParameters(float q_angle, float q_bias, float r_measure)
    {
        _q_angle = q_angle;
        _q_bias = q_bias;
        _r_measure = r_measure;
        pitchEKF.setNoiseParameters(q_angle, q_bias, r_measure);
        rollEKF.setNoiseParameters(q_angle, q_bias, r_measure);
        yawEKF.setNoiseParameters(q_angle, q_bias, r_measure);
        accXEKF.setNoiseParameters(q_angle, q_bias, r_measure);
        accYEKF.setNoiseParameters(q_angle, q_bias, r_measure);
        accZEKF.setNoiseParameters(q_angle, q_bias, r_measure);
    }

    float getRawPitch() const
    {
        return accPitch;
    }
    float getRawRoll() const
    {
        return accRoll;
    }

private:
    EKF pitchEKF;
    EKF rollEKF;
    EKF yawEKF;

    EKF accXEKF;
    EKF accYEKF;
    EKF accZEKF;

    float pitch;
    float roll;
    float yaw;

    float accXFiltered;
    float accYFiltered;
    float accZFiltered;

    float pitchOffset;
    float rollOffset;
    float yawOffset;

    float accX, accY, accZ;
    float accPitch, accRoll;
    float gyroX, gyroY, gyroZ;
    float gyroXOffset, gyroYOffset, gyroZOffset;

    float samplePeriod;
};
