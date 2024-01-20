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

		float Process();

		void SetFreq(float f);

		void SetAmp(float a);

		void SetSmooth(float s);

		void Update();

	private:
		Oscillator osc_;
		float sample_rate_;
		float cur_pos_ = 0.0f;
		float smoothing_ = 1.0f;
		float amp_ = 1.0f;
	};
}  // namespace touchgenerator
