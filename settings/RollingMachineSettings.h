#pragma once

//////////////////////////////////////////////
//    ローリングマシンの設定
//////////////////////////////////////////////
#define MOTOR_I2C_WIRE Wire1
#define MOTOR_I2C_ADDR ATOM_MOTION_I2C_ADDR
#define MOTOR_I2C_SDA ATOM_MOTION_SDA
#define MOTOR_I2C_SCL ATOM_MOTION_SCL

#define ANGLE_USE_ROLL true // 軸にAtomを交差させた場合はtrueにする
#define TUMBLE_DETECT false // 転倒検出を有効にする場合はtrueにする
// 転倒検出する角度(TUMBLE_DETECTがtrueの場合のみ有効)
#define PITCH_MIN -30.0f
#define PITCH_MAX 30.0f
#define ROLL_MIN -30.0f
#define ROLL_MAX 30.0f

// 1. モーターとサーボの設定
// 360度自由回転サーボを使う場合は、USE_SERVOをtrueにする
#define REVERSE true   // 回転方向を反転する場合はtrueにする
#define USE_SERVO false // サーボを使う場合はtrueにする
// サーボのチャンネルNo
#define SERVO_CH_LEFT 0  // 左サーボのチャンネル
#define SERVO_CH_RIGHT 1 // 右サーボのチャンネル
// DCモーターを使う場合は、USE_MOTORをtrueにする
#define USE_MOTOR true   // モーターを使う場合はtrueにする
#define MOTOR_CH_LEFT 0  // 左モーターのチャンネル
#define MOTOR_CH_RIGHT 1 // 右モーターのチャンネル

// 2. 目標角度(重要、基本0でOK)
// バランスがとれる角度に数値を設定するか、または
// 0を設定し、電源を切ってバランスをとった状態で電源を入れてキャリブレーションする
// 電源を切ってそのまま立てることができる角度を目標角度とする
// #define TARGET_ANGLE          -21.47f
#define TARGET_ANGLE 0.0f
#define CALIBRATE_ON_SETUP false // キャリブレーションを行う場合はtrueにする

// 3. PID制御の設定(重要、調整がマジで大変)
// (1)まずKI,KDの値を0にして、KPの値を大きくして、バランスをとる
// (2)次にKDの値を大きくして、振動がないように調整する
// (3)最後にKIの値を調整して、バランスをとる
#define KP 3 // 比例ゲイン反応を速くするために増やす
#define KI 0 // 積分ゲイン、小さめに設定
#define KD 0.5 // 微分ゲイン、適度に設定

#define SAMPLE_PERIOD 0.005

// 拡張カルマンフィルタ設定(基本は変更しなくてOK)
// 自己位置推定設定
#define EKF_ENABLE true // 拡張カルマンフィルタを有効にする
// 設定例
// 速度を優先する場合 0.01f, 0.003f, 0.03f
// 品質を優先する場合 0.001f, 0.0003f, 0.1f

// プロセスノイズ共分散行列の角度要素
// システムが角度に関してどれだけの不確実性を持っているかを示す
#define EKF_Q_ANGLE 0.02f
// プロセスノイズ共分散行列のバイアス要素
// システムがバイアスに関してどれだけの不確実性を持っているかを示す
#define EKF_Q_BIAS 0.006f
// 観測ノイズ共分散行列の角度要素
// 測定にどれだけのノイズが含まれているかを示します。
// 値が0である場合、測定にノイズがない（または無視できるほど小さい）と仮定する
#define EKF_R_MEASURE 0.03f
