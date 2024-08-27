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

#include "application.hpp"

#include <iomanip>

#include <clargs/parser.hpp>
#include <ruis/widget/button/base/push_button.hpp>

#include "spo2/contec_cms50d_plus.hpp"
#include "spo2/setocare_st_t130_u01.hpp"
#include "spo2/spo2_parameter_window.hpp"

#include "gui.hpp"
#include "settings_menu.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace bedsidemon;

application::application(bool window, std::string_view res_path) :
	ruisapp::application( //
		"ruis-tests",
		[]() {
			// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
			ruisapp::window_params wp(r4::vector2<unsigned>(1024, 600));
			return wp;
		}()
	)
{
	this->set_fullscreen(!window);

	this->gui.init_standard_widgets(*this->get_res_file());

	this->gui.context.get().loader.mount_res_pack(*this->get_res_file(papki::as_dir(res_path)));

	auto c = make_root_widgets(this->gui.context);

	this->menu_area = c.get().try_get_widget_as<ruis::container>("menu_area"sv);
	ASSERT(this->menu_area)

	// set up clock update
	{
		constexpr auto clock_update_interval_ms = 1000;

		auto& time_text_widget = c.get().get_widget_as<ruis::text>("clock_text"sv);

		this->clock_timer = utki::make_shared<ruis::timer>( //
			this->gui.context.get().updater,
			[this, time_text_widget = utki::make_shared_from(time_text_widget)](uint32_t elapsed_ms) {
				auto now = std::chrono::system_clock::now();
				auto time = std::chrono::system_clock::to_time_t(now);
				auto tm = *std::localtime(&time);

				std::stringstream ss;
				ss << std::put_time(&tm, "%T");

				time_text_widget.get().set_text(ss.str());

				this->clock_timer->stop();
				this->clock_timer->start(clock_update_interval_ms);
				ASSERT(this->clock_timer->is_running())
			}
		);
		this->clock_timer->start(0);
	}

	this->gui.set_root(c);

	auto& pw_container = c.get().get_widget_as<ruis::container>("pw_container");

	auto pw = utki::make_shared<spo2_parameter_window>(this->gui.context);
	// this->spo2_sensor_v = std::make_unique<contec_cms50d_plus>(pw,
	// "/dev/ttyUSB0"); NOLINTNEXTLINE(bugprone-unused-return-value, "false
	// positive")
	this->spo2_sensor_v = std::make_unique<setocare_st_t130_u01>(pw, "/dev/ttyUSB0");

	pw_container.push_back(pw);

	// set up buttons
	{
		c.get().get_widget_as<ruis::push_button>("home_button"sv).click_handler = [](ruis::push_button& b) {
			bedsidemon::application::inst().close_menu();
		};

		c.get().get_widget_as<ruis::push_button>("settings_button"sv).click_handler = [](ruis::push_button& b) {
			auto& app = bedsidemon::application::inst();
			app.open_menu(utki::make_shared<settings_menu>(app.gui.context));
		};
	}
}

std::unique_ptr<application> bedsidemon::create_application(std::string_view executable, utki::span<const char*> args)
{
	bool window = false;

	// TODO: look in /usr/local/share/bedsidemon first?
	std::string res_path = "/usr/share/bedsidemon"s;

	clargs::parser p;

	p.add("window", "run in window mode", [&]() {
		window = true;
	});

	p.add("res-path", "resources path, default = /usr/share/bedsidemon", [&](std::string_view v) {
		res_path = v;
	});

	p.parse(args);

	return std::make_unique<application>(window, res_path);
}

void bedsidemon::application::open_menu(utki::shared_ref<ruis::widget> menu)
{
	this->close_menu();

	this->menu = menu;

	this->menu_area->push_back(menu);
}

void bedsidemon::application::close_menu()
{
	if (!this->menu) {
		return;
	}
	this->menu->remove_from_parent();
	this->menu.reset();
}
