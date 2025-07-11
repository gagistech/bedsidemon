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

#include "application.hpp"

#include <iomanip>

#include <papki/fs_file.hpp>
#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/group/overlay.hpp>
#include <utki/config.hpp>
#include <utki/debug.hpp>

#if CFG_OS_NAME != CFG_OS_NAME_EMSCRIPTEN
#	include <clargs/parser.hpp>
#endif

#if CFG_OS_NAME != CFG_OS_NAME_EMSCRIPTEN
#	include "../sensors/spo2/contec_cms50d_plus.hpp"
#	include "../sensors/spo2/setocare_st_t130_u01.hpp"
#endif

#include "spo2/fake_spo2_sensor.hpp"
#include "spo2/spo2_parameter_window.hpp"

#include "about_menu.hpp"
#include "gui.hpp"
#include "quit_dialog.hpp"
#include "settings_menu.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace bedsidemon;

namespace {
constexpr std::string_view app_name = "bedsidemon"sv;

constexpr auto screen_width = 1024;
constexpr auto screen_height = 600;
} // namespace

application::application(bool window, std::string_view res_path) :
	ruisapp::application( //
		std::string(app_name),
		{
			.dims = {screen_width, screen_height},
			.title = std::string(app_name)
		}
	),
	res_path(papki::as_dir(res_path))
{
	this->set_fullscreen(!window);

	this->gui.init_standard_widgets(*this->get_res_file());

	this->gui.context.get().loader().mount_res_pack(*this->get_res_file(this->res_path));

	this->load_language(this->settings_storage.get().cur_language_index);

	auto c = make_root_widget(this->gui.context);

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

	// add fake sensor #1
	{
		constexpr auto deafult_color = 0xff00ff00;
		auto pw = utki::make_shared<spo2_parameter_window>(
			this->gui.context, //
			this->gui.context.get().localization.get().get("spo2_simulation"),
			deafult_color
		);
		this->fake_spo2_sensor_v = std::make_unique<fake_spo2_sensor>(
			pw, //
			utki::cat(papki::as_dir(res_path), "spo2_measurements.tml")
		);

		pw_container.push_back(pw);
	}

	// add fake sensor #2
	{
		constexpr auto deafult_color = 0xffff00ff;
		auto pw = utki::make_shared<spo2_parameter_window>(
			this->gui.context, //
			this->gui.context.get().localization.get().get("spo2_simulation"),
			deafult_color
		);
		this->fake_spo2_sensor2_v = std::make_unique<fake_spo2_sensor>(
			pw, //
			utki::cat(papki::as_dir(res_path), "spo2_measurements2.tml")
		);

		pw_container.push_back(pw);
	}

#if CFG_OS_NAME != CFG_OS_NAME_EMSCRIPTEN
	// add real sensor
	{
		auto pw = utki::make_shared<spo2_parameter_window>(this->gui.context);
		// this->real_spo2_sensor_v = std::make_unique<contec_cms50d_plus>(pw,
		// "/dev/ttyUSB0"); NOLINTNEXTLINE(bugprone-unused-return-value, "false
		// positive")
		this->real_spo2_sensor_v = std::make_unique<setocare_st_t130_u01>(pw, "/dev/ttyUSB0");

		pw_container.push_back(pw);
	}
#endif

	// set up buttons
	{
		c.get().get_widget_as<ruis::push_button>("home_button"sv).click_handler = [](ruis::push_button& b) {
			bedsidemon::application::inst().close_menu();
		};

		c.get().get_widget_as<ruis::push_button>("settings_button"sv).click_handler = [](ruis::push_button& b) {
			auto& app = bedsidemon::application::inst();
			app.open_menu(utki::make_shared<settings_menu>(app.gui.context));
		};

		c.get().get_widget_as<ruis::push_button>("about_button"sv).click_handler = [](ruis::push_button& b) {
			auto& app = bedsidemon::application::inst();
			app.open_menu(utki::make_shared<about_menu>(app.gui.context));
		};

		c.get().get_widget_as<ruis::push_button>("exit_button"sv).click_handler = [](ruis::push_button& b) {
			auto& app = bedsidemon::application::inst();
			auto& o = app.gui.get_root().get_widget<ruis::overlay>();
			o.context.get().post_to_ui_thread([&o]() {
				o.push_back(utki::make_shared<quit_dialog>(o.context));
			});
		};
	}
}

std::unique_ptr<application> bedsidemon::make_application(
	std::string_view executable, //
	utki::span<std::string_view> args
)
{
#if CFG_OS_NAME == CFG_OS_NAME_EMSCRIPTEN
	bool window = true;
	std::string res_path = "res/"s;
#else
	bool help = false;
	bool window = false;

	std::string res_path = []() {
		papki::fs_file local_share("/usr/local/share/bedsidemon/"sv);

		if (local_share.exists()) {
			return local_share.path();
		}

		return "/usr/share/bedsidemon/"s;
	}();

	clargs::parser p;

	p.add("help"s, "print command line interface help"s, [&]() {
		help = true;
	});

	p.add("window", "run in window mode", [&]() {
		window = true;
	});

	p.add("res-path", "resources path, default = /usr/share/bedsidemon/", [&](std::string_view v) {
		res_path = v;
	});

	p.parse(args);

	if (help) {
		std::cout << p.description() << std::endl;
		return nullptr;
	}
#endif

	return std::make_unique<application>(window, res_path);
}

void bedsidemon::application::open_menu(utki::shared_ref<bedsidemon::menu> menu)
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
	this->menu->on_close();
	this->menu.reset();
}

void application::load_language(size_t index)
{
	auto lng = settings::language_id_to_name_mapping.at(index).first;

	this->gui.context.get().localization = utki::make_shared<ruis::localization>(
		tml::read(*this->get_res_file(utki::cat(this->res_path, "localization/", lng, ".tml")))
	);
	this->gui.get_root().reload();
}
