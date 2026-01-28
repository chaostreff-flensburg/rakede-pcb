#include <avr/sleep.h>
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

//const uint16_t VCC_LOW_CUTOFF = 2800; // low voltage cutoff to protect Li-Ion cells
const uint16_t VCC_LOW_CUTOFF_MV = 2400; // low voltage cutoff to indicate that battery is really empty (NOT FOR Li-Ion!)
const uint16_t VCC_DIMMING_LOW_END_MV = 2800; // voltage above which globalBrightness is reduced to conserve energy
const uint32_t VCC_DIMMING_FACTOR = 200;


// soft PWM using micros() with target frequency of 100 Hz
// results in a total resolution of 10.000 split into 100 global x 100 individual
const uint8_t GLOBAL_BRIGHTNESS_MAX = 100;
const uint8_t INDIVIDUAL_BRIGHTNESS_MAX = 100;
uint8_t globalBrightness = GLOBAL_BRIGHTNESS_MAX;
uint8_t ledBrightness[LedIndexMax] = {0};


void setup() {
  for (int iLed = 0; iLed < LedIndexMax; iLed++) {
    pinMode(ledPins[iLed], OUTPUT);
    digitalWrite(ledPins[iLed], LOW);
  }

  analogReference(INTERNAL2V5); // set reference to the desired voltage, and set that as the ADC reference.
  analogReference(VDD); // Set the ADC reference to VDD. Voltage selected previously is still the selected, just not set as the ADC reference.
  uint16_t reading = analogRead(ADC_INTREF); //first reading might be inaccturate
  (void) reading; // to suppress unused variable warning

  // while(1) for (int iLed = 0; iLed < LedIndexMax; iLed++) {
  //   digitalWrite(iLed, HIGH);
  //   delay(1900);
  //   digitalWrite(iLed, LOW);
  //   delay(100);
  // }
}

void adaptToVcc() {
  const uint32_t intermediate = 1023ul * 2500;
  uint16_t reading = analogRead(ADC_INTREF);
  uint16_t vccMv = intermediate / reading;
  if (vccMv < VCC_LOW_CUTOFF_MV) {
    for (int iLed = 0; iLed < LedIndexMax; iLed++) {
      digitalWrite(ledPins[iLed], LOW);
      pinMode(ledPins[iLed], INPUT);
    }
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
  }
  //set globalBrightness according to Vcc
  if (vccMv > VCC_DIMMING_LOW_END_MV) {
    // antiproportional relation between Vcc and globalBrightness
    uint16_t brightness = GLOBAL_BRIGHTNESS_MAX * VCC_DIMMING_FACTOR
                          / (vccMv - VCC_DIMMING_LOW_END_MV + VCC_DIMMING_FACTOR);

    if (brightness > 100) {
      globalBrightness = 100;
    } else {
      globalBrightness = brightness;
    }
  } else {
    globalBrightness = 100;
  }
}


// animation timing like 'Leuchtturm Kalkgrund':
// (see https://www.deutsche-leuchtfeuer.de/ostsee/kalkgrund.html)
// Iso WRG 8 s   [4+(4)]
// 8 seconds for one rotation
// 4 seconds on, 4 senconds off
// fading from right to left (simulating clockwise rotation)

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
  const uint32_t ANIMATION_INTERVAL_US = 8000ul * 1000;

  static unsigned long animationStartUs = 0;
  
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
  const uint16_t FRAMETIME_US = 60ul * 1000;

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
  static bool needReCalculate = true;

  uint16_t usSincePulseStart = static_cast<uint16_t>(micros() - pulseStartUs);

  if (usSincePulseStart >= PULSE_INTERVAL_US) {
    // advance pulseStart_us, calculate animation and turn on LEDS if required 
    pulseStartUs += PULSE_INTERVAL_US;
    needReCalculate = true;

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
    if (needReCalculate && usSincePulseStart >= (PULSE_INTERVAL_US / 2)) {
      needReCalculate = false;

      adaptToVcc();
      animateLight(pulseStartUs);
      animateFlame(pulseStartUs);
    }
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
