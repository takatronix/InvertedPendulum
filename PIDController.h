#pragma once

class PIDController
{
public:
  // コンストラクタ。PIDゲインとサンプルタイムを設定する
  PIDController(float kp, float ki, float kd, int sampleTime = 100)
      : kp(kp), ki(ki), kd(kd), sampleTime(sampleTime), target(0), outputMin(0), outputMax(255), previousInput(0), integral(0)
  {
    lastTime = millis() - sampleTime; // 最後のサンプル時間を初期化
  }

  // PIDゲインを設定します。
  void setParameters(float kp, float ki, float kd)
  {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
  }

  // 目標値を設定します。
  void setTarget(float target)
  {
    this->target = target;
  }

  // 出力の最小値と最大値を設定します。
  void setOutputLimits(float min, float max)
  {
    this->outputMin = min;
    this->outputMax = max;
  }

  // サンプルタイムを設定します。
  void setSampleTime(int sampleTime)
  {
    this->sampleTime = sampleTime;
  }

  // PID制御計算を行います。入力値を受け取り、制御出力を返します。
  float compute(float input)
  {
    unsigned long now = millis();
    int timeChange = (now - lastTime);

    if (timeChange >= sampleTime)
    {
      float error = target - input; // 誤差計算
      integral += (ki * error);     // 積分計算
      if (integral > outputMax)
        integral = outputMax; // 積分の制限
      else if (integral < outputMin)
        integral = outputMin;

      float derivative = (input - previousInput); // 微分計算

      float output = kp * error + integral - kd * derivative; // PID計算

      // 出力の制限
      if (output > outputMax)
        output = outputMax;
      else if (output < outputMin)
        output = outputMin;

      previousInput = input; // 前回の入力を更新
      lastTime = now;        // 最後のサンプル時間を更新
      return output;
    }
    return 0; // 計算不要の場合
  }

private:
  float kp;               // 比例ゲイン
  float ki;               // 積分ゲイン
  float kd;               // 微分ゲイン
  float target;           // 目標値
  float outputMin;        // 出力の最小値
  float outputMax;        // 出力の最大値
  float previousInput;    // 前回の入力値
  float integral;         // 積分値
  int sampleTime;         // サンプルタイム
  unsigned long lastTime; // 最後のサンプル時間
};
