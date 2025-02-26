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

#include "spo2_sensor.hpp"

#ifdef BEDSIDEMON_RECORD_SPO2_MEASUREMENTS
#	include <tml/tree.hpp>
#	include <papki/fs_file.hpp>
#endif

using namespace std::string_view_literals;

using namespace bedsidemon;

spo2_sensor::spo2_sensor(utki::shared_ref<spo2_parameter_window> pw) :
	param_window(std::move(pw))
{}

void spo2_sensor::push(const spo2_measurement& meas)
{
#ifdef BEDSIDEMON_RECORD_SPO2_MEASUREMENTS
	this->record.push_back(meas);
#endif

	this->param_window.get().context.get().post_to_ui_thread([pw = this->param_window, meas]() {
		// std::cout << std::dec;
		// std::cout << "\t" << "signal_strength = " <<
		// unsigned(meas.signal_strength) << "\n"; std::cout << "\t" <<
		// "pulse_beep = " << meas.pulse_beat << "\n"; std::cout << "\t" <<
		// "finger_out = " << meas.finger_out << "\n"; std::cout << "\t" <<
		// "waveform_point = " << unsigned(meas.waveform_point) << "\n";
		// std::cout << "\t" << "pulse_rate = " << unsigned(meas.pulse_rate) <<
		// "\n"; std::cout << "\t" << "spo2 = " << unsigned(meas.spo2) << "\n";
		// std::cout << "\t" << "perfusion_index = " <<
		// unsigned(meas.perfusion_index) << "\n"; std::cout << "\t" <<
		// "delta_time = " << unsigned(meas.delta_time_ms) << "\n"; std::cout <<
		// std::endl;

		pw.get().set(meas);
	});
}

spo2_sensor::~spo2_sensor()
#ifdef BEDSIDEMON_RECORD_SPO2_MEASUREMENTS
{
	tml::forest rec;

	for (const auto& m : this->record) {
		rec.push_back(tml::tree frame("frame", m.to_tml()));
	}

	papki::fs_file fi("spo2_measurements.tml");
	tml::write(rec, fi);
}
#else
	= default;
#endif
