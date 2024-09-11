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

#include "fake_spo2_sensor.hpp"

#include <tml/tree.hpp>

#include "../application.hpp"

using namespace std::string_view_literals;

using namespace bedsidemon;

namespace {
std::vector<spo2_measurement> read_recorded_data(std::string_view data_filename)
{
	std::vector<spo2_measurement> ret;

	auto fi = bedsidemon::application::inst().get_res_file(data_filename);
	ASSERT(fi)

	auto frames = tml::read(*fi);

	for (const auto& f : frames) {
		if (f.value.to_string() != "frame"sv) {
			continue;
		}

		ret.push_back(spo2_measurement::from_tml(f.children));
	}

	if (ret.empty()) {
		throw std::invalid_argument("spo2 data file contains no frames");
	}

	return ret;
}
} // namespace

fake_spo2_sensor::fake_spo2_sensor(utki::shared_ref<spo2_parameter_window> pw, std::string_view data_filename) :
	spo2_sensor(pw),
	record(read_recorded_data(data_filename)),
	timer(
		utki::make_shared<ruis::timer>(
			pw.get().context.get().updater,
			[this](uint32_t elapsed_ms) {
				this->on_timer_expired(elapsed_ms);
			}
		)
	),
	cur_frame(this->record.begin())
{
	this->timer.get().start(this->cur_frame->delta_time_ms);
}

void fake_spo2_sensor::on_timer_expired(uint32_t elapsed_ms)
{
	auto dt_ms = this->time_since_frame_start_ms + elapsed_ms;

	while (dt_ms >= this->cur_frame->delta_time_ms) {
		this->push(*this->cur_frame);

		dt_ms -= this->cur_frame->delta_time_ms;

		++this->cur_frame;

		if (this->cur_frame == this->record.end()) {
			this->cur_frame = this->record.begin();
		}
	}

	ASSERT(dt_ms < this->cur_frame->delta_time_ms)

	this->time_since_frame_start_ms = dt_ms;

	auto time_to_wait_ms = this->cur_frame->delta_time_ms - dt_ms;

	auto& t = timer.get();
	t.stop();
	t.start(time_to_wait_ms);
}
