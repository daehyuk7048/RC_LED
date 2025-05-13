#include <PinChangeInterrupt.h>

// 채널 핀 정의
#define CH1 A0  // 밝기
#define CH2 A1  // 색상
#define CH5 A2  // on/off

// LED 핀
const int LED_R = 9;
const int LED_O = 10;
const int LED_G = 11;

// PWM 입력 저장용
volatile unsigned long startCH1, startCH2, startCH5;
volatile int pwmCH1 = 1500, pwmCH2 = 1500, pwmCH5 = 1500;
volatile bool newCH1 = false, newCH2 = false, newCH5 = false; 

void isrCH1() {//신대혁 작성
  if (digitalRead(CH1)) startCH1 = micros(); //high 감지 , 펄스 시작 
  else {
    pwmCH1 = micros() - startCH1; // 펄스 길이 계산
    newCH1 = true;
  }
}

void isrCH2() {//신대혁 작성
  if (digitalRead(CH2)) startCH2 = micros(); //high 감지 , 펄스 시작
  else {
    pwmCH2 = micros() - startCH2; // 펄스 길이 계산
    newCH2 = true;
  }
}

void isrCH5() {//신대혁 작성
  if (digitalRead(CH5)) startCH5 = micros(); //high 감지 , 펄스 시작
  else {
    pwmCH5 = micros() - startCH5; // 펄스 길이 계산
    newCH5 = true;
  }
}

void setup() {//신대혁, 김태현 작성
  Serial.begin(9600);

  pinMode(CH1, INPUT_PULLUP);
  pinMode(CH2, INPUT_PULLUP);
  pinMode(CH5, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(CH1), isrCH1, CHANGE);
  attachPCINT(digitalPinToPCINT(CH2), isrCH2, CHANGE);
  attachPCINT(digitalPinToPCINT(CH5), isrCH5, CHANGE);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_O, OUTPUT);
}


void loop() {//김태현 작성
  static bool ledOn = true;
  int brightness = map(pwmCH1, 1000, 2000, 0, 255); // 밝기 조절 맵핑 0-255

  if (newCH5) {
    ledOn = (pwmCH5 > 1500);  // CH5: 스위치 위치에 따라 ON/OFF
    newCH5 = false;
  }

  if (ledOn) {
    if (pwmCH2 < 1300) {
      analogWrite(LED_R, brightness); // PWM 1300 높으면 red light // brightness 값으로 밝기 조절
      analogWrite(LED_G, 0);
      analogWrite(LED_O, 0);
    } else if (pwmCH2 < 1700) { 
      analogWrite(LED_R, 0);
      analogWrite(LED_G, brightness); // PWM 1700 높으면 orange light // brightness 값으로 밝기 조절
      analogWrite(LED_O, 0);
    } else {
      analogWrite(LED_R, 0);
      analogWrite(LED_G, 0);
      analogWrite(LED_O, brightness); // PWM 1300 ~ 1700 orange light // brightness 값으로 밝기 조절
    }
  } else {
    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_O, 0);
  }

  newCH1 = newCH2 = false;
}
