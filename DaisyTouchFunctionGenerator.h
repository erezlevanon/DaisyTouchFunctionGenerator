#pragma once

#include "simple-daisy-touch.h"
#include "DaisyDuino.h"
#include "Adafruit_MPR121.h"
#include <Arduino.h>


#define NUM_SEGMENTS 8

using namespace synthux;
using namespace simpletouch;

namespace touchgenerator {

	class TouchGenerator {
	public:
		//
		void Init(float sample_rate, int scl, int sda);

		// Init with default i2c connections (daisy i2c4).
		void Init(float sample_rate);

		void SetRange(float min_val, float max_val);

		float Process();

		void SetFreq(float f);

		void SetAmp(float a);

		// Set the smoothing cooeficient for the generated function.
		// accepted values are from 0 to 1.
		void SetSmooth(float s);

		void Update();

	private:
		void OnPadTouch(uint16_t pad);
		void OnPadRelease(uint16_t pad);

		Touch touch_;
		Oscillator osc_;
		float sample_rate_;
		float cur_pos_ = 0.0f;
		float smoothing_ = 1.0f;
		float amp_ = 1.0f;
		float min_val_ = -1.0f;
		float max_val_ = 1.0f;

		// A represntation of the current set generated functino / waveform.
		float cur_func_[NUM_SEGMENTS] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

		// Utility arrays used to calculate cur_func from the touch seqeunce.
		size_t vals_[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		size_t starts_[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		bool recording_touch_sequence = false;
	};
}  // namespace touchgenerator
