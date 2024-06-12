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

void runLight() {
  const int intervalMs = 33;
  const int restartIntervalMs = 3000;

  static unsigned long next = 0;
  static unsigned long nextRestart = 0;
  static int state = 0;

  if (millis() > next) {
    switch(state) {
    case 0:
      digitalWrite(PIN_LIGHT_R, HIGH);
      state = 1;
      break;

    case 1:
      digitalWrite(PIN_LIGHT_R, LOW);
      digitalWrite(PIN_LIGHT_C, HIGH);
      state = 2;
      break;

    case 2:
      digitalWrite(PIN_LIGHT_C, LOW);
      digitalWrite(PIN_LIGHT_L, HIGH);
      state = 3;
      break;

    case 3:
      digitalWrite(PIN_LIGHT_L, LOW);
      state = 4;
      break;

    default:
      break;
    }

    if (millis() > nextRestart) {
      state = 0;
      nextRestart += restartIntervalMs;
    }

    next += intervalMs;
  }
}

void runFlame() {
  const uint8_t pwmDim = 192, pwmBright = 255;
  const int intervalMs = 33;

  static unsigned long next = 0;
  static int state = 0;

  if (millis() > next) {
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

    next += intervalMs;
  }
}

void loop() {
  runLight();
  runFlame();
}
