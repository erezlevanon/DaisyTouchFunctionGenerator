#pragma once

#include "simple-daisy-touch.h"
#include <DaisyDuino.h>
#include <Adafruit_MPR121.h>
#include <Arduino.h>


#define NUM_SEGMENTS 8

using namespace synthux;
using namespace simpletouch;

namespace touchgenerator {

	class TouchGenerator {
	public:
		// Needs to be called exactly once, usually in setup.
		void Init(float sample_rate, int scl, int sda);

		// Needs to be called exactly once, usually in setup.
		// Default i2c connections (daisy i2c4).
		void Init(float sample_rate);

		// Set the min an max output values of the function.
		// min_val can be larger then max_val to create an inverse function.
		void SetRange(float min_val, float max_val);

		// Should be called to output the current value of the function and
		// move to the next step.
		float Process();

		// Set the frequency of the generated function in hz.
		void SetFreq(float f);

		// Set the amplitude of the generated function.
		// Values should be non negative.
		void SetAmp(float a);

		// Set the smoothing cooeficient for the generated function.
		// accepted values are from 0 to 1.
		void SetSmooth(float s);

		// Update the current touch sequence, and if necessary the generated
		// funtion.
		// Should be called periodiaclly, usually in the arduino loop() function.
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

		// Utility vars used to calculate cur_func from the touch seqeunce.
		size_t vals_[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		size_t starts_[NUM_SEGMENTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		bool recording_touch_sequence = false;
	};
}  // namespace touchgenerator
