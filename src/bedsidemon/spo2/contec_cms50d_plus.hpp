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

#include "../serial_port_thread.hpp"

#include "spo2_sensor.hpp"

namespace bedsidemon {

class contec_cms50d_plus :
	public spo2_sensor, //
	private serial_port_thread
{
	enum class state {
		disconnected,
		idle,
		read_packet_high_bits,
		read_packet,

		enum_size
	};

	state state_v = state::idle;

	enum class packet_type {
		live_data,

		enum_size
	};

	struct packet {
		uint8_t high_bits{};
		packet_type type{packet_type::enum_size};
		size_t num_bytes_to_read{};
		std::vector<uint8_t> buffer;
	} packet_v;

	bool is_sending = false;

	unsigned num_live_data_packages_received = 0;

	uint32_t last_ticks{};

public:
	contec_cms50d_plus(utki::shared_ref<spo2_parameter_window> pw, std::string_view port_filename);

	contec_cms50d_plus(const contec_cms50d_plus&) = delete;
	contec_cms50d_plus& operator=(const contec_cms50d_plus&) = delete;

	contec_cms50d_plus(contec_cms50d_plus&&) = delete;
	contec_cms50d_plus& operator=(contec_cms50d_plus&&) = delete;

	~contec_cms50d_plus() override;

private:
	void on_data_received(utki::span<const uint8_t> data) override;
	void on_data_sent() override;
	void on_port_closed() override;

	void request_live_data(uint32_t cur_ticks);

	void feed(uint8_t byte);
	void handle_packet_type_byte(uint8_t byte);
	void handle_packet_byte(uint8_t byte);

	void apply_packet_high_bits();
	void handle_packet();
};

} // namespace bedsidemon
