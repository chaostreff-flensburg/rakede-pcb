enum {
  LedIndexLightL,
  LedIndexLightC,
  LedIndexLightR,
  LedIndexFlameI,
  LedIndexFlameO,
  LedIndexMax
};

const int ledPins[LedIndexMax] = {
  PIN_PA6, // LightL, no hard PWM
  PIN_PA7, // LightC
  PIN_PA1, // LightR
  PIN_PA3, // FlameI
  PIN_PA2  // FlameO
};

// soft PWM using micros() with target frequency of 100 Hz
// results in a total resolution of 10.000 split into 100 global x 100 individual
const uint8_t GLOBAL_BRIGHTNESS_MAX = 100;
const uint8_t INDIVIDUAL_BRIGHTNESS_MAX = 100;
uint8_t globalBrightness = GLOBAL_BRIGHTNESS_MAX;
uint8_t ledBrightness[LedIndexMax] = {0};


void setup() {
  for (int iLed = 0; iLed < LedIndexMax; iLed++) {
    pinMode(iLed, OUTPUT);
  }

  // for (int iLed = 0; iLed < LedIndexMax; iLed++) {
  //   digitalWrite(iLed, HIGH);
  //   delay(900);
  //   digitalWrite(iLed, LOW);
  //   delay(100);
  // }
}

void animateLightBlip(uint16_t msSinceStart) {
  if (msSinceStart < 100) {
    ledBrightness[LedIndexLightR] = 100;
    ledBrightness[LedIndexLightC] = 0;
    ledBrightness[LedIndexLightL] = 0;
  } else if (msSinceStart < 200) {
    ledBrightness[LedIndexLightR] = 100;
    ledBrightness[LedIndexLightC] = 100;
    ledBrightness[LedIndexLightL] = 0;
  } else if (msSinceStart < 4000) {
    ledBrightness[LedIndexLightR] = 100;
    ledBrightness[LedIndexLightC] = 100;
    ledBrightness[LedIndexLightL] = 100;
  } else if (msSinceStart < 4100) {
    ledBrightness[LedIndexLightR] = 0;
    ledBrightness[LedIndexLightC] = 100;
    ledBrightness[LedIndexLightL] = 100;
  } else if (msSinceStart < 4200) {
    ledBrightness[LedIndexLightR] = 0;
    ledBrightness[LedIndexLightC] = 0;
    ledBrightness[LedIndexLightL] = 100;
  } else {
    ledBrightness[LedIndexLightR] = 0;
    ledBrightness[LedIndexLightC] = 0;
    ledBrightness[LedIndexLightL] = 0;
  }
}

void animateLight(unsigned long nowUs) {
  const uint32_t ANIMATION_INTERVAL_US = 8000ul * 1000ul;

  static unsigned long animationStartUs = 0;
  
  // animation timing like 'Leuchtturm Kalkgrund':
  // (see https://www.deutsche-leuchtfeuer.de/ostsee/kalkgrund.html)
  // Iso WRG 8 s   [4+(4)]
  // 8 seconds for one rotation
  // 4 seconds on, 4 senconds off
  // fading from right to left (simulating clockwise rotation)

  uint32_t usSinceAnimationStart = nowUs - animationStartUs;
  if (usSinceAnimationStart > ANIMATION_INTERVAL_US) {
    animationStartUs += ANIMATION_INTERVAL_US;
    usSinceAnimationStart -= ANIMATION_INTERVAL_US;
  }

  animateLightBlip(usSinceAnimationStart / 1000);
}

void animateFlame(unsigned long nowUs) {
  const uint8_t PWM_DIM = 66;
  const uint8_t PWM_BRIGHT = 100;
  const uint16_t FRAMETIME_US = 60ul * 1000ul;

  static unsigned long nextFrameUs = 0;
  static int state = 0;

  if (nowUs > nextFrameUs) {
    nextFrameUs += FRAMETIME_US;

    switch (state) {
    case 0:
      ledBrightness[LedIndexFlameI] = PWM_DIM;
      ledBrightness[LedIndexFlameO] = PWM_DIM;
      state = 1;
      break;

    case 1:
      ledBrightness[LedIndexFlameI] = PWM_BRIGHT;
      ledBrightness[LedIndexFlameO] = PWM_DIM;
      state = 2;
      break;

    case 2:
    default:
      ledBrightness[LedIndexFlameI] = PWM_DIM;
      ledBrightness[LedIndexFlameO] = PWM_BRIGHT;
      state = 0;
      break;
    }
  }
}

void loop() {
  //drive LEDs
  const uint16_t PULSE_INTERVAL_US = static_cast<uint16_t>(INDIVIDUAL_BRIGHTNESS_MAX) * static_cast<uint16_t>(GLOBAL_BRIGHTNESS_MAX);

  static unsigned long pulseStartUs = 0;

  uint16_t usSincePulseStart = static_cast<uint16_t>(micros() - pulseStartUs);

  if (usSincePulseStart >= PULSE_INTERVAL_US) {
    // advance pulseStart_us, calculate animation and turn on LEDS if required 
    pulseStartUs += PULSE_INTERVAL_US;

    animateLight(pulseStartUs);
    animateFlame(pulseStartUs);

    if (globalBrightness > 0) {
      if (ledBrightness[LedIndexLightL] > 0)
        digitalWriteFast(ledPins[LedIndexLightL], HIGH);
      if (ledBrightness[LedIndexLightC] > 0)
        digitalWriteFast(ledPins[LedIndexLightC], HIGH);
      if (ledBrightness[LedIndexLightR] > 0)
        digitalWriteFast(ledPins[LedIndexLightR], HIGH);
      if (ledBrightness[LedIndexFlameI] > 0)
        digitalWriteFast(ledPins[LedIndexFlameI], HIGH);
      if (ledBrightness[LedIndexFlameO] > 0)
        digitalWriteFast(ledPins[LedIndexFlameO], HIGH);
    }
  } else {
    // turn off LEDs according to their brightness
    if (usSincePulseStart > ledBrightness[LedIndexLightL] * globalBrightness)
      digitalWriteFast(ledPins[LedIndexLightL], LOW);
    if (usSincePulseStart > ledBrightness[LedIndexLightC] * globalBrightness)
      digitalWriteFast(ledPins[LedIndexLightC], LOW);
    if (usSincePulseStart > ledBrightness[LedIndexLightR] * globalBrightness)
      digitalWriteFast(ledPins[LedIndexLightR], LOW);
    if (usSincePulseStart > ledBrightness[LedIndexFlameI] * globalBrightness)
      digitalWriteFast(ledPins[LedIndexFlameI], LOW);
    if (usSincePulseStart > ledBrightness[LedIndexFlameO] * globalBrightness)
      digitalWriteFast(ledPins[LedIndexFlameO], LOW);
  }
}
