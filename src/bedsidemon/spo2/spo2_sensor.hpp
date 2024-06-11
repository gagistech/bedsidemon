/*
bedsidemon - Bed-side monitor example GUI project

Copyright (C) 2024 Gagistech Oy <gagistechoy@gmail.com>

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

#include <mutex>

#include "spo2_measurement.hpp"
#include "spo2_parameter_window.hpp"

namespace bedsidemon {

class spo2_sensor
{
	utki::shared_ref<spo2_parameter_window> param_window;

public:
	spo2_sensor(utki::shared_ref<spo2_parameter_window> pw);

	spo2_sensor(const spo2_sensor&) = delete;
	spo2_sensor& operator=(const spo2_sensor&) = delete;

	spo2_sensor(spo2_sensor&&) = delete;
	spo2_sensor& operator=(spo2_sensor&&) = delete;

	virtual ~spo2_sensor() = default;

protected:
	// thread safe
	void push(const spo2_measurement& meas);
};

} // namespace bedsidemon
