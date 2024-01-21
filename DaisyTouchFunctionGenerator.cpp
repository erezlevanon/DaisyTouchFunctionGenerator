#include "DaisyTouchFunctionGenerator.h"

#include "simple-daisy-touch.h"
#include "DaisyDuino.h"
#include "Adafruit_MPR121.h"
#include <Arduino.h>

using namespace synthux;
using namespace simpletouch;

namespace touchgenerator {

	namespace {

		// A "segemented" linear interpulation function.
		// while a classic lerp function will give you a value between a and b in proportion to x,
		// this function simulates dividing the range between a and b to s segements, transition from a to b
		// and returns the value of the bucket where x falls.
		// if s <= 1, will return a,
		// if s is infinity, will behave like classic lerp function. 
		// This was meant to work with a, b and x between 0 and 1, and with s >= 1.
		// Other values might have undefined behavior.
		float slerp(float a, float b, float x, float s) {
			if (s <= 1) return a;
			return (a * s + floor(x * s * (b - a) / 10.0) * 10.0) / s;
		}

	}  // namespace


	void TouchGenerator::Init(float sample_rate, int scl, int sda) {
		sample_rate_ = sample_rate;

		osc_.Init(sample_rate_);
		osc_.SetWaveform(Oscillator::WAVE_RAMP);
		osc_.SetAmp(0.5f - 0.001f);
		osc_.SetFreq(880.0f);

		touch_.Init(scl, sda);
		touch_.SetOnTouch([&](uint16_t pad) -> void {OnPadTouch(pad); });
		touch_.SetOnRelease([&](uint16_t pad) -> void {OnPadRelease(pad); });
	}

	// Init with default i2c connections (daisy i2c4).
	void TouchGenerator::Init(float sample_rate) {
		Init(sample_rate, D13, D14);
	}

	void TouchGenerator::OnPadTouch(uint16_t pad) {
		const bool has_touch = touch_.HasTouch();
		if (!recording_touch_sequence && has_touch) {
			Serial.println("Recording Function");
			for (int i = 0; i < NUM_SEGMENTS; i++) {
				vals_[i] = 0;
				starts_[i] = 0;
			}
		}
		if (pad < NUM_SEGMENTS) {
			starts_[pad] = millis();
		}
		recording_touch_sequence = has_touch;
	}

	void TouchGenerator::OnPadRelease(uint16_t pad) {
		const bool has_touch = touch_.HasTouch();
		if (recording_touch_sequence && !has_touch) {
			size_t cur_max = 0;
			size_t cur_millis = millis();
			for (int i = 0; i < NUM_SEGMENTS; i++) {
				vals_[i] = starts_[i] == 0 ? 0 : millis() - starts_[i];
				if (vals_[i] > cur_max) cur_max = vals_[i];
			}
			if (cur_max == 0) {
				Serial.println("missfire");
				return;
			}

			for (int i = 0; i < NUM_SEGMENTS; i++) {
				cur_func_[i] = float(vals_[i]) / float(cur_max * 0.5f) - 1.0f;
			}

			// Print generated function
			for (int i = 0; i < NUM_SEGMENTS; i++) {
				Serial.print(cur_func_[i], 2);
				Serial.print("-");
			}
			Serial.println();
		}
		recording_touch_sequence = has_touch;
	}

	float TouchGenerator::Process() {
		cur_pos_ = (osc_.Process() + 0.5f) * float(NUM_SEGMENTS);

		float prev = cur_func_[int(cur_pos_)];
		float next = cur_func_[int(cur_pos_ + 1) % NUM_SEGMENTS];
		float interpulation = cur_pos_ - int(cur_pos_);

		return slerp(prev, next, interpulation, smoothing_) * amp_;
	}

	void TouchGenerator::SetFreq(float f) {
		osc_.SetFreq(f);
	}

	void TouchGenerator::SetAmp(float a) {
		amp_ = a;
	}

	void TouchGenerator::SetSmooth(float s) {
		smoothing_ = powf(s, 4) * 1800.0f + 1.00001f;
	}

	void TouchGenerator::Update() {
		touch_.Process();
	}
}  // namespace touchgenerator
