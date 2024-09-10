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

	this->param_window.get().context.get().run_from_ui_thread([pw = this->param_window, meas]() {
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

	for(const auto& m : this->record){
		tml::tree f("frame", {});

		auto& c = f.children;

		c.push_back(tml::tree("strength"sv, {tml::leaf(m.signal_strength)}));
		c.push_back(tml::tree("beat"sv, {tml::leaf(m.pulse_beat)}));
		c.push_back(tml::tree("finger_out"sv, {tml::leaf(m.finger_out)}));
		c.push_back(tml::tree("wf_value"sv, {tml::leaf(m.waveform_point)}));
		c.push_back(tml::tree("pulse"sv, {tml::leaf(m.pulse_rate)}));
		c.push_back(tml::tree("spo2"sv, {tml::leaf(m.spo2)}));
		c.push_back(tml::tree("perf"sv, {tml::leaf(m.perfusion_index)}));
		c.push_back(tml::tree("dt_ms"sv, {tml::leaf(m.delta_time_ms)}));

		rec.push_back(std::move(f));
	}

	papki::fs_file fi("spo2_measurements.tml");
	tml::write(rec, fi);
}
#else
= default;
#endif
