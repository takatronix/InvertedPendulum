# 倒立振子（とうりつしんし）とは

M5StickC/AtomなどのマイコンとLEGOで使えるモーター、LEGOパーツを使って倒立振子作りました。
倒立振子（とうりつしんし）とは、重心が支点の上にある不安定なシステムで、簡単に言えば「逆さまに立っている振り子」です。
セグウェイとか人が乗れる２輪の乗り物などもそれにあたります。


## 仕組み

・M5マイコンのIMU(慣性計測ユニット)から得た、加速度計とジャイロスコープの６軸の情報を使って姿勢(傾き)を推定する
・傾きからモーターの出力をPID制御(比例・積分・微分制御)で水平に戻すように調整する


## 利用したもの

IMU搭載のM5マイコンとI2C接続のサーボドライバがあれば設定変更だけで動くとは思いますが、私が購入したものを紹介します。


### バランスマシン１号(360度自由回転サーボ)
M5StickCPlus2(CPU)
https://docs.m5stack.com/ja/core/M5StickC%20PLUS2
C Back Driver(サーボドライバー)
https://docs.m5stack.com/ja/hat/c_back_driver
GeekServo(連続回転サーボとタイヤ)
https://www.amazon.co.jp/gp/product/B0CC4RG5ZQ
レゴテクニックパーツ
https://www.lego.com/ja-jp/themes/technic

### ローリングマシン(DCモーター版)
M5AtomS3(CPU)
https://docs.m5stack.com/ja/core/AtomS3
Atom Motion(モータードライバー)
https://docs.m5stack.com/ja/atom/atom_motion
GeekServo DCモーター
https://www.amazon.co.jp/dp/B0B3LXS13W
レゴテクニックパーツ
https://www.lego.com/ja-jp/themes/technic

### 依存ライブラリ
M5UI(自作のUIライブラリ)
https://github.com/takatronix/M5UI


### 設定方法
モーターやPID制御の設定はsettingsの下に分離しており、モーターの種類や接続などを設定した後に、



# 今後の予定
M5StickCPlusとジョイスティックHATでリモートコントロール