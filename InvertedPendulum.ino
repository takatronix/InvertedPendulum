#include <M5UI.h>
#include "AtomicMotion.h"
#include "DualButtonUnit.h"
#include "OrientationSensor.h"
#include "PIDController.h"

#include "FileManager.h"
#include "SettingsManager.h"

FileManager fileManager;
SettingsManager settingsManager("/config/settings.json");

// 設定値
#include "settings/DefaultSettings.h"
// #include "settings/RollingMachineSettings.h"

// 姿勢センサー
OrientationSensor imu;
// モーター制御
AtomicMotion Motion;
// ボタン入力
DualButtonUnit DualButton;
// PID制御
PIDController pid(KP, KI, KD, SAMPLE_PERIOD);

// offscreen buffer
M5UICanvas screen(&M5.Display);

void setup()
{
    Serial.begin(115200);
    Serial.println("Start Inverted Pendulum");
    M5.begin();

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

    DualButton.begin();


    if (!fileManager.begin()) {
        Serial.println("Failed to initialize SPIFFS");
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

    // CSVファイルの書き込み
    std::vector<std::vector<String>> csvData = {
        {"Name", "Age", "City"},
        {"Alice", "30", "New York"},
        {"Bob", "25", "Los Angeles"},
        {"Charlie", "35", "Chicago"}
    };
    if (fileManager.writeCSV("/data.csv", csvData)) {
        Serial.println("CSV file written successfully");
    } else {
        Serial.println("Failed to write CSV file");
    }

    // CSVファイルの読み込み
    std::vector<std::vector<String>> readData = fileManager.readCSV("/data.csv");
    Serial.println("CSV file contents:");
    for (const auto& row : readData) {
        for (const auto& cell : row) {
            Serial.print(cell);
            Serial.print(" ");
        }
        Serial.println();
    }


    // ファイルリストの一覧を表示
    Serial.println("Listing files...");
    fileManager.listFiles("/");

    // SPIFFSの残り容量を表示
    Serial.println("Printing SPIFFS info...");
    fileManager.printSPIFFSInfo();
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

    // モーターの出力を設定
    setMotorSpeed(output);

    // 画面表示
    screen.clear();
    screen.setCursor(0, 0);
    screen.printf("Angle %.2f\n", currentAngle);
    screen.printf("Pitch %.2f\n", imu.getPitch(EKF_ENABLE));
    screen.printf("Roll  %.2f\n", imu.getRoll(EKF_ENABLE));
    screen.printf("Poffset %.2f\n", imu.getPitchOffset());
    screen.printf("Roffset %.2f\n", imu.getRollOffset());
    screen.printf("OUT:%d\n", (int)output);
    screen.printf("FPS:%d Draw:%dms\n", screen.getFPS(), screen.getDrawingTime());

    if (M5.BtnA.wasPressed())
    {
        screen.fillScreen(RED);
    }

    if (M5.BtnA.wasHold())
    {
        screen.fillScreen(BLUE);
        screen.update();
        Motion.stopAllMotors();
        imu.calibrate();
        return;
    }

    screen.update();
    delay(SAMPLE_PERIOD * 1000 - screen.getDrawingTime());
}

void setMotorSpeed(int speed)
{
    if (REVERSE)
    {
        speed = -speed;
    }
    if (USE_SERVO)
    {
        Motion.setServoSpeed(SERVO_CH_LEFT, speed);
        Motion.setServoSpeed(SERVO_CH_RIGHT, -speed);
    }
    if (USE_MOTOR)
    {
        Motion.setMotorSpeed(MOTOR_CH_LEFT, speed);
        Motion.setMotorSpeed(MOTOR_CH_RIGHT, -speed);
    }
}
