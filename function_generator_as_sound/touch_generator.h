#pragma once

#include "simple-daisy-touch.h"
#include "DaisyDuino.h"
#include "Adafruit_MPR121.h"
#include <Arduino.h>


#define NUM_SEGMENTS 8

using namespace synthux;
using namespace simpletouch;

namespace touchgenerator {

float lerp(float a, float b, float x, float s) {
  return (a * s + floor(x * s * (b - a) / 10.0) * 10.0) / s;
}

// namespace {
static size_t vals[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static size_t starts[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static float cur_func[NUM_SEGMENTS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

static bool prev_state = false;

static Touch touch;
// } // namespace
void OnPadTouch(uint16_t pad) {
  const bool cur_state = touch.HasTouch();
  if (!prev_state && cur_state) {
    Serial.println("start");
    for (int i = 0; i < NUM_SEGMENTS; i++) {
      vals[i] = 0;
      starts[i] = 0;
    }
  }
  if (pad < NUM_SEGMENTS) {
    starts[pad] = millis();
  }
  prev_state = cur_state;
}

void OnPadRelease(uint16_t pad) {
  const bool cur_state = touch.HasTouch();
  if (prev_state && !cur_state) {
    size_t cur_max = 0;
    size_t cur_millis = millis();
    for (int i = 0; i < NUM_SEGMENTS; i++) {
      vals[i] = starts[i] == 0 ? 0 : millis() - starts[i];
      if (vals[i] > cur_max) cur_max = vals[i];
    }
    if (cur_max == 0) {
      Serial.println("missfire");
      return;
    }
    float cumsum = 0.0f;

    for (int i = 0; i < NUM_SEGMENTS; i++) {
      cur_func[i] = float(vals[i]) / float(cur_max * 0.5f) - 1.0f;
      cumsum += cur_func[i];
    }
    
    // Normalize wave to have integral of 1.0
    for (int i = 0; i < NUM_SEGMENTS; i++) {
      // cur_func[i] = cur_func[i] / cumsum;
      Serial.print(cur_func[i], 2);
      Serial.print("-");
    }
    Serial.println();
  }
  prev_state = cur_state;
}

class TouchGenerator {
public:
  void Init(float sample_rate) {
    Serial.println("initing");
    sample_rate_ = sample_rate;

    osc_.Init(sample_rate_);
    osc_.SetWaveform(Oscillator::WAVE_RAMP);
    osc_.SetAmp(0.5f - 0.001f);
    osc_.SetFreq(880.0f);

    touch.Init();
    touch.SetOnTouch(OnPadTouch);
    touch.SetOnRelease(OnPadRelease);
  }

  float Process() {
    cur_pos_ = (osc_.Process() + 0.5f) * float(NUM_SEGMENTS);

    float prev = cur_func[int(cur_pos_)];
    float next = cur_func[int(cur_pos_ + 1) % NUM_SEGMENTS];
    float interpulation = cur_pos_ - int(cur_pos_);
    
    return lerp(prev, next, interpulation, smoothing_) * amp_;
  }

  void SetFreq(float f) {
    osc_.SetFreq(f);
  }

  void SetAmp(float a) {
    amp_ = a;
  }

  void SetSmooth(float s) {
    smoothing_ = s * 1800.0f + 200.0f;
  }

  void Update() {
    touch.Process();
  }

private:
  Oscillator osc_;
  float sample_rate_;
  float cur_pos_ = 0.0f;
  float smoothing_ = 1.0f;
  float amp_ = 1.0f;
};
}  // namespace touchgenerator
