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
#include <ruis/widget/base/color_widget.hpp>
#include <ruis/widget/label/text.hpp>
#include <ruis/widget/widget.hpp>

#include "../settings.hpp"
#include "../waveform.hpp"

#include "spo2_measurement.hpp"
#include "spo2_parameter_window_menu.hpp"

namespace bedsidemon {

class spo2_parameter_window :
	virtual public ruis::widget, //
	public ruis::color_widget,
	private ruis::container
{
	friend class spo2_parameter_window_menu;

	ruis::text& spo2_value;
	ruis::text& bpm_value;
	ruis::widget& heart;

	bedsidemon::waveform& waveform;

	utki::shared_ref<ruis::timer> heart_timer;

	void trigger_heart();
	void on_heart_timer_expired();

	utki::signal<const settings&>::connection settings_change_signal_connection;

	std::shared_ptr<spo2_parameter_window_menu> pw_menu;

public:
	spo2_parameter_window(utki::shared_ref<ruis::context> context, std::u32string title = std::u32string(U"SpO2 %"));
	~spo2_parameter_window() override;

	spo2_parameter_window(const spo2_parameter_window&) = delete;
	spo2_parameter_window& operator=(const spo2_parameter_window&) = delete;

	spo2_parameter_window(spo2_parameter_window&&) = delete;
	spo2_parameter_window& operator=(spo2_parameter_window&&) = delete;

	void set(const spo2_measurement& meas);

	void on_color_change() override;

	// clang-format off
	constexpr static const std::array<uint32_t, 4> possible_colors = {{
		 0xffffffff,
		 0xffffff00,
		 0xff00ff00,
		 0xff00ffff
	}};
	// clang-format on
};

} // namespace bedsidemon
