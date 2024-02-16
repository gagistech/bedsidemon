/*
bedsidemon - Bed-side monitor example GUI project

Copyright (C) 2024 Ivan Gagis  <igagis@gmail.com>

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

#include "../serial_port_thread.hpp"

#include "spo2_sensor.hpp"

namespace bedsidemon {

class setocare_st_t130_u01 :
	public spo2_sensor, //
	private serial_port_thread
{
	enum class state {
		disconnected,
		wait_packet_first_byte,
		read_packet,

		enum_size
	};

	state state_v = state::disconnected;

	struct packet {
		size_t num_bytes_to_read;
		std::vector<uint8_t> buffer;
	} packet_v;

	uint32_t last_ticks;

public:
	setocare_st_t130_u01(utki::shared_ref<spo2_parameter_window> pw, std::string_view port_filename);

private:
	void on_data_received(utki::span<const uint8_t> data) override;

	void on_data_sent() override {}

	void on_port_closed() override;

	void feed(uint8_t byte);

	void handle_packet();
};

} // namespace bedsidemon
