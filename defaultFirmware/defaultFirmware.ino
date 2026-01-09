const int PIN_LIGHT_R = PIN_PA1;
const int PIN_FLAME_O = PIN_PA2;
const int PIN_FLAME_I = PIN_PA3;
const int PIN_LIGHT_L = PIN_PA6; // no PWM
const int PIN_LIGHT_C = PIN_PA7;

void setup() {
  pinMode(PIN_LIGHT_R, OUTPUT);
  pinMode(PIN_FLAME_O, OUTPUT);
  pinMode(PIN_FLAME_I, OUTPUT);
  pinMode(PIN_LIGHT_L, OUTPUT);
  pinMode(PIN_LIGHT_C, OUTPUT);

  // digitalWrite(PIN_LIGHT_R, HIGH);
  // delay(500);
  // digitalWrite(PIN_LIGHT_R, LOW);

  // digitalWrite(PIN_LIGHT_C, HIGH);
  // delay(500);
  // digitalWrite(PIN_LIGHT_C, LOW);

  // digitalWrite(PIN_LIGHT_L, HIGH);
  // delay(500);
  // digitalWrite(PIN_LIGHT_L, LOW);

  // digitalWrite(PIN_FLAME_I, HIGH);
  // delay(500);
  // digitalWrite(PIN_FLAME_I, LOW);

  // digitalWrite(PIN_FLAME_O, HIGH);
  // delay(500);
  // digitalWrite(PIN_FLAME_O, LOW);
}

void animateLightBlip(int offsetMs) {
  if (offsetMs < 100) {
    digitalWrite(PIN_LIGHT_R, HIGH);
    digitalWrite(PIN_LIGHT_C, LOW);
    digitalWrite(PIN_LIGHT_L, LOW);
  } else if (offsetMs < 200) {
    digitalWrite(PIN_LIGHT_R, HIGH);
    digitalWrite(PIN_LIGHT_C, HIGH);
    digitalWrite(PIN_LIGHT_L, LOW);
  } else if (offsetMs < 4000) {
    digitalWrite(PIN_LIGHT_R, HIGH);
    digitalWrite(PIN_LIGHT_C, HIGH);
    digitalWrite(PIN_LIGHT_L, HIGH);
  } else if (offsetMs < 4100) {
    digitalWrite(PIN_LIGHT_R, LOW);
    digitalWrite(PIN_LIGHT_C, HIGH);
    digitalWrite(PIN_LIGHT_L, HIGH);
  } else if (offsetMs < 4200) {
    digitalWrite(PIN_LIGHT_R, LOW);
    digitalWrite(PIN_LIGHT_C, LOW);
    digitalWrite(PIN_LIGHT_L, HIGH);
  } else {
    digitalWrite(PIN_LIGHT_R, LOW);
    digitalWrite(PIN_LIGHT_C, LOW);
    digitalWrite(PIN_LIGHT_L, LOW);
  }
}

void animateLight() {
  const int framerateMs = 33;
  const int animationDurationMs = 8000;

  static unsigned long nextFrame = 0;
  static unsigned long animationStartMs = 0;
  
  // animation timing like 'Leuchtturm Kalkgrund':
  // (see https://www.deutsche-leuchtfeuer.de/ostsee/kalkgrund.html)
  // Iso WRG 8 s   [4+(4)]
  // 8 seconds for one rotation
  // 4 seconds on, 4 senconds off
  // fading from right to left (simulating clockwise rotation)

  if (millis() > nextFrame) {
    nextFrame += framerateMs;
    int sinceLastStartMs = millis() - animationStartMs;
    if (sinceLastStartMs > animationDurationMs) {
      animationStartMs += animationDurationMs;
    } else {
      animateLightBlip(sinceLastStartMs);
    }
  }
}

void animateFlame() {
  const uint8_t pwmDim = 170, pwmBright = 255;
  const int framerateMs = 60;

  static unsigned long nextFrame = 0;
  static int state = 0;

  if (millis() > nextFrame) {
    switch (state) {
    case 0:
      analogWrite(PIN_FLAME_I, pwmDim);
      analogWrite(PIN_FLAME_O, pwmDim);
      state = 1;
      break;

    case 1:
      analogWrite(PIN_FLAME_I, pwmBright);
      analogWrite(PIN_FLAME_O, pwmDim);
      state = 2;
      break;

    case 2:
    default:
      analogWrite(PIN_FLAME_I, pwmDim);
      analogWrite(PIN_FLAME_O, pwmBright);
      state = 0;
      break;
    }

    nextFrame += framerateMs;
  }
}

void loop() {
  animateLight();
  animateFlame();
}
