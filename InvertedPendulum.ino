#include <M5UI.h>

#include "OrientationSensor.h"
#include "PIDController.h"

#include "FileManager.h"
#include "SettingsFile.h"


FileManager fileManager;
SettingsFile settings("/config/settings.json");

// 設定値
//#include "settings/DefaultSettings.h"
#include "settings/GreenServoSettings.h"
// #include "settings/RollingMachineSettings.h"

// 姿勢センサー
OrientationSensor imu;
// モーター制御
HatCBack motor;
//Hat8Servos_v1_1 motor;

// ボタン入力
//DualButtonUnit DualButton;
// PID制御
PIDController pid(KP, KI, KD, SAMPLE_PERIOD);

// offscreen buffer
M5UICanvas screen(&M5.Display);

//UnitAngle8  unitAngle;
//UnitCardKB  unitCardKB;
//UnitMiniScales unitScale;
//UnitSonic sonic;

#define MOTOR_SPEED_MIN -127
#define MOTOR_SPEED_MAX 127
//M5_ANGLE8 angle8;
//UnitMiniJoyC joyC;

void setup()
{
    LOG_I("Start InvertedPendulum");
    auto cfg = M5.config();
   // cfg.pmic_button = false;  // これで電源ボタン状態取得を行わなくなる
    //cfg.internal_imu = false; // これで内蔵IMUを使わなくなる
   // cfg.internal_rtc = false; // これで内蔵RTCを使わなくなる
    M5.begin(cfg);
    //Wire.end();
    //Wire1.end();
    //Wire1.end();
    LOG_D("M5.begin()");
  //  motor.begin(ATOM_MOTION_I2C_ADDR, ATOM_MOTION_SDA, ATOM_MOTION_SCL);
 //   motor.begin(0x36, 0, 26);
   // Wire.begin(0, 26);

/*
    pinMode(26, OUTPUT);
    pinMode(0, OUTPUT);
    M5.In_I2C.release();
    M5.In_I2C.begin(0x38,0, 26);
*/
    auto sda = M5.Ex_I2C.getSDA();
    auto scl = M5.Ex_I2C.getSCL();
   // motor.begin(0x36, sda, scl);

    motor.begin();

    fileManager.begin();

    // オフスクリーンバッファの初期化
    screen.enableRotation = false;

    screen.setup();
    screen.removeAllRenderers();
    screen.setCursor(0, 0);
    screen.setTextSize(1);
    screen.setTextColor(GREEN);

    screen.setRotation(2);

    // PID制御の初期化
    pid.setOutputLimits(MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    pid.setTarget(TARGET_ANGLE);

    // 位置推定処理の初期化
    imu.setSamplePeriod(SAMPLE_PERIOD);
    imu.setNoiseParameters(EKF_Q_ANGLE, EKF_Q_BIAS, EKF_R_MEASURE);
    if (CALIBRATE_ON_SETUP)
    {
        imu.calibrate();
    }


    //DualButton.begin();

/*
    settings["KP"] = KP;
    settings["KI"] = KI;
    settings["KD"] = KD;
    settings["SAMPLE_PERIOD"] = SAMPLE_PERIOD;
    settings["TARGET_ANGLE"] = TARGET_ANGLE;
    settings["EKF_ENABLE"] = EKF_ENABLE;
    settings["EKF_Q_ANGLE"] = EKF_Q_ANGLE;
    settings["EKF_Q_BIAS"] = EKF_Q_BIAS;
    settings["EKF_R_MEASURE"] = EKF_R_MEASURE;

*/
/*
    if (!fileManager.begin()) {
        LOG_E("Failed to initialize SPIFFS");
        return;  // SPIFFSの初期化に失敗した場合、処理を中断
    }

    // 設定値を保存
    Serial.println("Saving settings...");
    StaticJsonDocument<200> saveDoc;
    saveDoc["wifi_ssid"] = "your_ssid";
    saveDoc["wifi_password"] = "your_password";
    settingsManager.saveSettings(saveDoc.as<JsonObject>());

    // ファイルが存在するか確認
    if (fileManager.fileExists("/config.json")) {
        Serial.println("File exists after saving.");
    } else {
        Serial.println("File does not exist after saving.");
    }

    // 保存されたファイルの内容を表示
    settingsManager.printFileContent();

    // 設定値を読み込み
    Serial.println("Loading settings...");
    StaticJsonDocument<200> loadDoc;
    if (settingsManager.loadSettings(loadDoc)) {
        const char* ssid = loadDoc["wifi_ssid"];
        const char* password = loadDoc["wifi_password"];
        Serial.printf("SSID: %s, Password: %s\n", ssid, password);
    } else {
        Serial.println("Failed to load settings or settings are empty");
    }

    // 設定値を削除
    Serial.println("Deleting settings...");
    if (settingsManager.deleteSettings()) {
        Serial.println("Settings deleted successfully");
    } else {
        Serial.println("Failed to delete settings");
    }

    // ファイルが存在するか確認
    if (fileManager.fileExists("/config.json")) {
        Serial.println("File still exists after deleting.");
    } else {
        Serial.println("File does not exist after deleting.");
    }


    // ファイルリストの一覧を表示
    Serial.println("Listing files...");
    fileManager.listFiles("/");

    // SPIFFSの残り容量を表示
    Serial.println("Printing SPIFFS info...");
    fileManager.printSPIFFSInfo();
    */
   LOG_I("End InvertedPendulum");
   
/*
   Wire.end();
   // SDA,SCLを表示
    LOG_D("SDA:%d SCL:%d", M5.Ex_I2C.getSDA(), M5.Ex_I2C.getSCL());
    LOG_D("SDA:%d SCL:%d", sda, scl);
    while (!unitAngle.begin(sda,scl,ANGLE8_I2C_ADDR)) {
        LOG_E("UnitAngle8 not found");
        delay(100);
    }
*/
   // unitAngle.begin();
    //sonic.begin();
   // unitAngle.begin();
    //unitCardKB.begin();
    Serial.println("angle8 Connect Success");

    //unitScale.begin();

    //joyC.begin();
   // unitAngle.begin();
}

void loop()
{
    M5.update();
    screen.start();
    imu.update();

    // 現在の角度(Pitch)を取得
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
    
    /*
        unitAngle.update();
        // unitAngleの値をサーボモーターに出力

    uint8_t input = unitAngle.input[0];
    int angle = map(input, 0, 255,SERVO_PULSE_MIN,SERVO_PULSE_MAX);
    motor.setServoPulse(0, angle);


    uint8_t input2 = unitAngle.input[1];
    int angle2 = map(input2, 0, 255,SERVO_PULSE_MIN,SERVO_PULSE_MAX);
    motor.setServoPulse(1, angle2);

    uint8_t input3 = unitAngle.input[2];
    int angle3 = map(input3, 0, 255,SERVO_PULSE_MIN,SERVO_PULSE_MAX);
    motor.setServoPulse(2, angle3);

    uint8_t input4 = unitAngle.input[3];
    int angle4 = map(input4, 0, 255,SERVO_PULSE_MIN,SERVO_PULSE_MAX);
    motor.setServoPulse(3, angle4);

*/
    // joyCから値を取得
 //   joyC.update();
    //unitAngle.update();

  // setMotorSpeed(output);
 //   int key = unitCardKB.getKey();
    //float weight = unitScale.getWeight();
   // unitScale.setLEDColor(0xff,0xff,0xff);
    // 画面表示
    screen.clear();
    screen.setCursor(0, 0);
   // screen.printf("Weight:%.2f\n", weight);
  //  screen.printf("Distance:%.2f\n", d);
   // screen.printf("key:%d\n", key);
    screen.printf("FPS:%d Draw:%dms\n", screen.getFPS(), screen.getDrawingTime());

    screen.printf("Pitch %.2f\n", imu.getPitch(EKF_ENABLE));
    screen.printf("Roll  %.2f\n", imu.getRoll(EKF_ENABLE));
    screen.printf("Poffset %.2f\n", imu.getPitchOffset());
    screen.printf("Roffset %.2f\n", imu.getRollOffset());
//    screen.printf("OUT:%d FPS:%d Draw:%dms\n", (int)output,);
    //screen.printf("EX SDA:%2d SCL:%2d\n", M5.Ex_I2C.getSDA(), M5.Ex_I2C.getSCL());
  //  screen.printf("IN SDA:%2d SCL:%2d\n", M5.In_I2C.getSDA(), M5.In_I2C.getSCL());
    if (M5.BtnA.wasPressed())
    {
        screen.fillScreen(RED);
    }

    if (M5.BtnA.wasHold())
    {
        screen.fillScreen(BLUE);
        screen.update();
        //motor.stopAllMotors();
        imu.calibrate();
        return;
    }
    screen.update();

 //   LOG_D("Angle:%.2f Pitch:%.2f Roll:%.2f Poffset:%.2f Roffset:%.2f OUT:%d", currentAngle, imu.getPitch(EKF_ENABLE), imu.getRoll(EKF_ENABLE), imu.getPitchOffset(), imu.getRollOffset(), (int)output);
    delay(SAMPLE_PERIOD * 1000 );
    //unitScale.update();

    //LOG_D("FPS:%d Draw:%dms", screen.getFPS(), screen.getDrawingTime());
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


