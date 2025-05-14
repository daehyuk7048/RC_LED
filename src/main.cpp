#include <PinChangeInterrupt.h>

/* ── 0. 함수 프로토타입 ───────────────── */
void rgbOut(int r, int g, int b);

/* ── 1. RC 채널 ↔ 핀 매핑 ───────────────────── */
/*   CH5 → A2 : LED-A ON/OFF      */
/*   CH2 → A1 : LED-B 밝기        */
/*   CH1 → A0 : RGB LED-C 색상    */
#define CH_SWITCH  A2      // ★ LED-A ON/OFF  (CH5)
#define CH_BRIGHT  A1      // ★ LED-B 밝기    (CH2)
#define CH_COLOR   A0      // ★ LED-C 색상    (CH1)

/* ── 2. LED 출력 핀 ─────────────────────────── */
const int LED_A   = 4;     // 단색 LED-A (온/오프)
const int LED_B   = 3;     // 단색 LED-B (PWM 밝기)
const int LED_R   = 9;     // RGB-LED-C : R
const int LED_G   = 10;    //              G
const int LED_BLU = 11;    //              B

/* ── 3. 펄스 측정 전역변수 ──────────────────── */
volatile unsigned long stSwitch, stBright, stColor;
volatile int  pwSwitch = 1500, pwBright = 1500, pwColor = 1500;
volatile bool fSwitch  = false, fBright = false, fColor = false;

/* ── 4. ISR 세 개 ──────────────────────────── */
void isrSwitch(){ if (digitalRead(CH_SWITCH)) stSwitch = micros();
                  else { pwSwitch = micros() - stSwitch; fSwitch = true; } }

void isrBright(){ if (digitalRead(CH_BRIGHT)) stBright = micros();
                  else { pwBright = micros() - stBright; fBright = true; } }

void isrColor (){ if (digitalRead(CH_COLOR )) stColor  = micros();
                  else { pwColor  = micros() - stColor ; fColor  = true; } }

/* ── 5. 초기화 ─────────────────────────────── */
void setup(){
  Serial.begin(9600);

  pinMode(CH_SWITCH, INPUT_PULLUP);
  pinMode(CH_BRIGHT, INPUT_PULLUP);
  pinMode(CH_COLOR , INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(CH_SWITCH), isrSwitch, CHANGE);
  attachPCINT(digitalPinToPCINT(CH_BRIGHT), isrBright, CHANGE);
  attachPCINT(digitalPinToPCINT(CH_COLOR ), isrColor , CHANGE);

  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_BLU, OUTPUT);
}

/* ── 6. 메인 루프 ──────────────────────────── */
void loop(){
  static bool ledA_on = true;          // LED-A 상태

  /* 6-1. LED-A : ON/OFF (CH5 → A2) */
  if (fSwitch){
    ledA_on = (pwSwitch > 1500);       // 스위치 위쪽이면 켜짐
    digitalWrite(LED_A, ledA_on ? HIGH : LOW);
    fSwitch = false;
  }

  /* 6-2. LED-B : 밝기 (CH2 → A1) */
  if (fBright){
    int duty = map(pwBright, 1000, 2000, 0, 255);
    analogWrite(LED_B, constrain(duty, 0, 255));
    fBright = false;
  }

  /* 6-3 RGB-LED-C ── 색상 전용 (CH1 → A0) */
    if (fColor) {
      long hue = map(pwColor, 1000, 2000, 0, 765);      // 0~765(=3*255) 삼각 무지개
      int r=0, g=0, b=0;
  
      if (hue < 255) {            // 빨→노
        r = 255;
        g = hue;
      } else if (hue < 510) {     // 노→초
        long h = hue - 255;
        r = 255 - h;
        g = 255;
      } else {                    // 초→청→파
        long h = hue - 510;
        g = 255 - h;
        b = h;
      }
      rgbOut(r, g, b);
      fColor = false;
    }

  /* (디버그용) 시리얼 출력 ------------------- */
  if (fSwitch || fBright || fColor){
    Serial.print("Sw:"); Serial.print(pwSwitch);
    Serial.print("  Br:"); Serial.print(pwBright);
    Serial.print("  Cl:"); Serial.println(pwColor);
    fSwitch = fBright = fColor = false;
  }

  // delay(1);   // CPU 여유
}

/* ── 7. RGB 헬퍼 함수 ─────────────────────── */
void rgbOut(int r, int g, int b){      // 공통캐소드 기준
  analogWrite(LED_R,   r);
  analogWrite(LED_G,   g);
  analogWrite(LED_BLU, b);
}
