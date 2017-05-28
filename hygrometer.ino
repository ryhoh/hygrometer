// Copyright (c) 2017 Tetsuya Hori
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php

/* 
 *  Tetsuya Hori rewritten on 2017/05/28
 *  
 *  ------------------------------------------
 *  Using DHT library by Adafruit.
 *    DHT library
 *      MIT license
 *      written by Adafruit Industries  
 *  ------------------------------------------
 *  
 *  7セグメントLEDアノード側の、シフトレジスタと各LEDとの対応
 * 
 *  bit 0bxxxxxxxx
 *  LED   BAFEDCGx
 * 
 */

#include "DHT.h"

#define DATA 2
#define LATCH 3
#define CLOCK 4

#define DYNAMIC_TIME 3  // ダイナミック駆動の間隔(ms)
#define BRIGHTNESS 5   // 数値が大きいほど暗くなる

#define ON_TIME 3000
#define OFF_TIME 7000

//DHT dht(DHTPIN, DHTTYPE);
DHT dht1(12, DHT11);
DHT dht2(11, DHT11);

// カソード側トランジスタの接続ポート
const int tr[4] = {5, 6, 7, 8};

// 表示パターン
const int led[10] = {0b11111100, 0b10000100, 0b11011010, 0b11001110, 0b10100110, 0b01101110, 0b01111110, 0b11100100, 0b11111110, 0b11101110};
const int char_E = 0b01111010;
const int char_r = 0b00010010;

void setup() {
  int i;

  pinMode(DATA, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  for(i = 0; i < 4; i++){
    pinMode(tr[i], OUTPUT);
  }

  
  dht1.begin();
  dht2.begin();
  // Wait a few seconds between measurements.
  delay(2000);
}

void loop() {
  static int i, hum[2];
  hum[0] = (int)dht1.readHumidity();
  hum[1] = (int)dht2.readHumidity();
  
  unsigned long timer = millis();
  while(millis() < timer + ON_TIME){
    for(i = 0; i < 2; i++){
      if((hum[i] == 0) || isnan(hum[i])){   // 現実的には湿度0%はありえないのでエラー扱い
        putNumber(char_r, tr[0 + 2*i]);     // 不安定な電源では0%になることが多々あるため
        putNumber(char_E, tr[1 + 2*i]);
      } else {
        putNumber(led[hum[i] % 10], tr[0 + 2*i]);
        putNumber(led[hum[i] / 10], tr[1 + 2*i]);
      }
      delay(BRIGHTNESS);  // 表示間隔を延ばすことでLEDの明るさを調節
    }
  }
  delay(OFF_TIME);
}

void putNumber(int x, int trNumber){
  shiftSet(x);    // [重要]シフトレジスタをセットしてからトランジスタのONOFFをする
  digitalWrite(trNumber, HIGH);
  delay(DYNAMIC_TIME);
  digitalWrite(trNumber, LOW);
  delay(1);
}

void shiftSet(int x){
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, x);
  digitalWrite(LATCH, HIGH);
}

