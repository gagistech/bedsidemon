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

#include <ruis/util/timer.hpp>
#include <ruisapp/application.hpp>

#include "spo2/spo2_sensor.hpp"

namespace bedsidemon {

class application : public ruisapp::application
{
	std::unique_ptr<spo2_sensor> spo2_sensor_v;

	// timer for updating clock view once a second
	std::shared_ptr<ruis::timer> clock_timer;

public:
	application(bool window, std::string_view res_path);
};

std::unique_ptr<application> create_application(std::string_view executable, utki::span<const char*> args);

} // namespace bedsidemon
