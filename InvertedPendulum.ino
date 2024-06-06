#include <M5UI.h>
#include "OrientationSensor.h"
#include "PIDController.h"

#include "FileManager.h"
#include "SettingsFile.h"

// 設定値
//#include "settings/DefaultSettings.h"
#include "settings/GreenServoSettings.h"
//#include "settings/RedMotorSettings.h"
//#include "settings/RollingMachineSettings.h"


// 姿勢センサー
OrientationSensor imu;
// PID制御
PIDController pid(KP, KI, KD, SAMPLE_PERIOD);
// モーター
MotorDriver motor;

// 設定ファイル
SettingsFile settings("/config/settings.json");

// offscreen buffer
M5UICanvas screen(&M5.Display);
// バッテリー残量表示
BatterySprite battery(&screen);
// 距離センサー
UnitToF tof;

// 停止中か？
bool isStopped = false;

//　背景色
uint16_t backgroundColor = TFT_BLACK;
//FileManager fileManager;
// 
void setup()
{
    LOG_I("Start InvertedPendulum");
    auto cfg = M5.config();
    M5.begin(cfg);

    // 設定ファイルの初期化 
    settings.begin();
    //fileManager.begin();
    //settings.load();
    // モーターの初期化
    motor.begin(MOTOR_I2C_WIRE, MOTOR_I2C_ADDR, MOTOR_I2C_SDA, MOTOR_I2C_SCL);

    // startup sound
    Sound::playNote(Note::E5,100);
    Sound::playNote(Note::A3,100);

    // オフスクリーンバッファの初期化
    screen.enableRotation = false;
    screen.setup();
    screen.removeAllRenderers();

    // PID制御の初期化
    pid.setOutputLimits(MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    pid.setTarget(TARGET_ANGLE);

    // 位置推定処理の初期化
    imu.setSamplePeriod(SAMPLE_PERIOD);
    imu.setNoiseParameters(EKF_Q_ANGLE, EKF_Q_BIAS, EKF_R_MEASURE);
    if (CALIBRATE_ON_SETUP)
    {
        imu.calibrate();
      // imu.load(settings);
    }
       imu.calibrate();
 
    tof.begin();

    LOG_I("Setup done");
}

void loop()
{
   
    M5.update();
    screen.start();
    tof.update();

    // 現在の傾き(Pitch/Roll)を取得
    imu.update();
    float pitch = imu.getPitch(EKF_ENABLE);
    float roll = imu.getRoll(EKF_ENABLE);

    // 軸にAtomを交差させた場合はRollを使用
    float currentAngle = ANGLE_USE_ROLL ? roll : pitch;

    // PID制御を行い、モーターの出力を計算
    float output = pid.compute(currentAngle);
   
    // 転倒時、裏返した時はモーターを停止させる
    if (TUMBLE_DETECT)
    {
        if (pitch < PITCH_MIN || pitch > PITCH_MAX)
        {
            output = 0;
        }
        if (roll < ROLL_MIN || roll > ROLL_MAX)
        {
            output = 0;
        }
    }


    if (M5.BtnA.wasPressed())
    {
        if(isStopped){
            isStopped = false;
        }
        else{
            Stop();
        }
    }

    if (M5.BtnA.wasHold())
    {
        screen.fillScreen(BLUE);
        screen.update();
        Sound::beep();
        stopMotors();
        delay(3000);

        screen.fillScreen(RED);
        screen.update();

        imu.calibrate();
        imu.save(settings);
    
        Sound::playNote(Note::C4,100);
        Sound::playNote(Note::E4,100);

        screen.fillScreen(RED);
        screen.update();
        return;
    }

    if(isStopped){
        output = 0;
        setColor(0, 80, 0);
    }else{
        setColor(abs(output) + 80, 0, 0);
    }
    // モーターの出力を設定
    setMotorSpeed(output);


    // 画面表示
    screen.fillScreen(backgroundColor);
    screen.setCursor(0, 0);
    screen.printf("FPS:%d Draw:%dms\n", screen.getFPS(), screen.getDrawingTime());
    screen.printf("Distance:%d\n", tof.distance);
    screen.printf("Pitch %.2f\n", pitch);
    screen.printf("Roll  %.2f\n", roll);
    screen.printf("Poffset %.2f\n", imu.getPitchOffset());
    screen.printf("Roffset %.2f\n", imu.getRollOffset());

    //tof.update();
    screen.update();

    delay(SAMPLE_PERIOD * 1000 );
}

void setColor(int r, int g, int b)
{
    backgroundColor = M5.Lcd.color565(r, g, b);
}


void setMotorSpeed(int speed)
{
    if (REVERSE)
    {
        speed = -speed;
    }
    if (USE_SERVO)
    {
        motor.setServoSpeed(SERVO_CH_LEFT, speed);
        motor.setServoSpeed(SERVO_CH_RIGHT, -speed);
    }
    if (USE_MOTOR)
    {
        motor.setMotorSpeed(MOTOR_CH_LEFT, speed);
        motor.setMotorSpeed(MOTOR_CH_RIGHT, -speed);
    }
}

void stopMotors(){
    if (USE_SERVO)
    {
        motor.setServoSpeed(SERVO_CH_LEFT, 0);
        motor.setServoSpeed(SERVO_CH_RIGHT, 0);
    }
    if (USE_MOTOR)
    {
        motor.setMotorSpeed(MOTOR_CH_LEFT, 0);
        motor.setMotorSpeed(MOTOR_CH_RIGHT, 0);
    }
}

void Stop()
{
    isStopped = true;
    stopMotors();
}

