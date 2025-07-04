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

#include <ruis/util/timer.hpp>

#include "spo2_sensor.hpp"

namespace bedsidemon {

class fake_spo2_sensor : public spo2_sensor
{
	std::vector<spo2_measurement> record;

	utki::shared_ref<ruis::timer> timer;

	decltype(record)::const_iterator cur_frame;

	void on_timer_expired(uint32_t elapsed_ms);
	uint32_t time_since_frame_start_ms = 0;

public:
	fake_spo2_sensor(
		utki::shared_ref<spo2_parameter_window> pw, //
		std::string_view data_filename
	);

	fake_spo2_sensor(const fake_spo2_sensor&) = delete;
	fake_spo2_sensor& operator=(const fake_spo2_sensor&) = delete;

	fake_spo2_sensor(fake_spo2_sensor&&) = delete;
	fake_spo2_sensor& operator=(fake_spo2_sensor&&) = delete;

	~fake_spo2_sensor() override = default;

private:
};

} // namespace bedsidemon
