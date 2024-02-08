#pragma once

#include <cstdint>

namespace bedsidemon {

struct spo2_measurement {
	bool pulse_beat;

	bool finger_out;

	float waveform_point;

	uint8_t pulse_rate; // 0xff = invalid

	float spo2; // oxygenation, %, >100 = invalid

	uint16_t perfusion_index; // in 0.01 of %

	uint16_t delta_time_ms;
};

} // namespace bedsidemon
