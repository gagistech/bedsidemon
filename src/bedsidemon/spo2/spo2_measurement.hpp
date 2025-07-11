/*
bedsidemon - Bedside monitor example GUI project

Copyright (C) 2024-2025 Gagistech Oy <gagistechoy@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <cstdint>

#include <tml/tree.hpp>

namespace bedsidemon {

struct spo2_measurement {
	uint8_t signal_strength; // in %

	bool pulse_beat;

	bool finger_out;

	float waveform_point;

	uint8_t pulse_rate; // 0xff = invalid

	uint8_t spo2; // oxygenation, %, >100 = invalid

	uint16_t perfusion_index; // in 0.01 of %

	// time from previous sample
	uint16_t delta_time_ms;

	tml::forest to_tml() const;

	static spo2_measurement from_tml(const tml::forest& f);
};

} // namespace bedsidemon
