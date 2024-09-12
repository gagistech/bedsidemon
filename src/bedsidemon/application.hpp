/*
bedsidemon - Bedside monitor example GUI project

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

#include "menu.hpp"
#include "settings.hpp"

namespace bedsidemon {

class application : public ruisapp::application
{
	std::shared_ptr<ruis::container> menu_area;
	std::shared_ptr<bedsidemon::menu> menu;

	std::unique_ptr<spo2_sensor> fake_spo2_sensor_v;
	std::unique_ptr<spo2_sensor> real_spo2_sensor_v;

	// timer for updating clock view once a second
	std::shared_ptr<ruis::timer> clock_timer;

public:
	bedsidemon::settings_storage settings_storage;

	application(bool window, std::string_view res_path);

	static application& inst()
	{
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast, "here we are 100% sure the cast is safe")
		return static_cast<application&>(ruisapp::application::inst());
	}

	void open_menu(utki::shared_ref<bedsidemon::menu> menu);
	void close_menu();
};

std::unique_ptr<application> create_application(std::string_view executable, utki::span<const char*> args);

} // namespace bedsidemon
