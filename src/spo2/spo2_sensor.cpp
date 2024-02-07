/*
bedsidemon - Bed-side monitor example GUI project

Copyright (C) Ivan Gagis  <igagis@gmail.com>

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

#include "spo2_sensor.hpp"

using namespace bedsidemon;

spo2_sensor::spo2_sensor(utki::shared_ref<spo2_parameter_window> pw) :
	param_window(std::move(pw))
{}

void spo2_sensor::push(const measurement& meas)
{
	this->param_window.get().context.get().run_from_ui_thread([pw = this->param_window, meas]() {
		std::cout << "\t" << "pulse_beep = " << meas.pulse_beat << "\n";
		std::cout << "\t" << "finger_out = " << meas.finger_out << "\n";
		std::cout << "\t" << "waveform_point = " << unsigned(meas.waveform_point) << "\n";
		std::cout << "\t" << "pulse_rate = " << unsigned(meas.pulse_rate) << "\n";
		std::cout << "\t" << "spo2 = " << unsigned(meas.spo2) << "\n";
		std::cout << "\t" << "perfusion_index = " << unsigned(meas.perfusion_index) << "\n";
        std::cout << "\t" << "delta_time = " << unsigned(meas.delta_time_ms) << "\n";
		std::cout << std::endl;
	});
}
