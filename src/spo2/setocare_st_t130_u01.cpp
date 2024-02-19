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

#include "setocare_st_t130_u01.hpp"

#include <utki/time.hpp>

using namespace bedsidemon;

namespace {
constexpr auto serial_port_baud_rate = baud_rate::baud_4800;

constexpr auto bci_protocol_packet_size = 5;
} // namespace

setocare_st_t130_u01::setocare_st_t130_u01(utki::shared_ref<spo2_parameter_window> pw, std::string_view port_filename) :
	spo2_sensor(std::move(pw)),
	serial_port_thread(port_filename, serial_port_baud_rate)
{
	this->state_v = state::wait_packet_first_byte;
}

void setocare_st_t130_u01::on_port_closed()
{
	this->state_v = state::disconnected;
}

void setocare_st_t130_u01::on_data_received(utki::span<const uint8_t> data)
{
	// std::cout << "data received" << std::endl;

	for (const auto& b : data) {
		this->feed(b);
	}
}

void setocare_st_t130_u01::feed(uint8_t byte)
{
	// std::cout << "byte received, state = " << unsigned(this->state_v) << std::endl;
	switch (this->state_v) {
		case state::disconnected:
			// ignore any data, it is not supposed to come from disconnected port
			break;
		case state::wait_packet_first_byte:
			if (!(byte & utki::bit_7_mask)) {
				// not a packet first byte,
				// need to wait for next packet start,
				// ignore
				break;
			}
			ASSERT(this->packet_v.buffer.empty())
			this->packet_v.buffer.push_back(byte);
			this->packet_v.num_bytes_to_read = bci_protocol_packet_size - 1;
			this->state_v = state::read_packet;
			break;
		case state::read_packet:
			if (byte & utki::bit_7_mask) {
				std::cout << "unexpected packet start encountered, previous packet length was only "
						  << this->packet_v.buffer.size() << " bytes" << std::endl;
				this->packet_v.buffer.clear();
				this->state_v = state::wait_packet_first_byte;
				this->feed(byte);
				break;
			}

			this->packet_v.buffer.push_back(byte);
			--this->packet_v.num_bytes_to_read;
			if (this->packet_v.num_bytes_to_read == 0) {
				this->state_v = state::wait_packet_first_byte;
				this->handle_packet();
				this->packet_v.buffer.clear();
			}
			break;
		case state::enum_size:
			ASSERT(false)
			break;
	}
}

void setocare_st_t130_u01::handle_packet()
{
	const auto& buf = this->packet_v.buffer;

	ASSERT(buf.size() == bci_protocol_packet_size)

	// std::cout << "packet received =";
	// for (const auto& b : buf) {
	// 	std::cout << " 0x" << std::hex << unsigned(b);
	// }
	// std::cout << std::endl;

	uint8_t signal_strength = buf[0] & utki::lower_nibble_mask;
	// bool no_signal = buf[0] & utki::bit_4_mask;
	// bool probe_unplugged = buf[0] & utki::bit_5_mask;
	bool pulse_beep = buf[0] & utki::bit_6_mask;

	uint8_t pleth = buf[1] & (~utki::bit_7_mask);

	// uint8_t bar_graph = buf[2] & utki::lower_nibble_mask;
	bool no_finger = buf[2] & utki::bit_4_mask;
	// bool pulse_search = buf[2] & utki::bit_5_mask;

	uint8_t pulse_rate = ((buf[2] & utki::bit_6_mask) << 1) | (buf[3] & (~utki::bit_7_mask));
	uint spo2 = buf[4] & (~utki::bit_7_mask);

	// std::cout << std::dec;
	// std::cout << "signal_strength = " << unsigned(signal_strength) << "\n";
	// std::cout << "\t" << "no_signal = " << no_signal << "\n";
	// std::cout << "\t" << "probe_unplugged = " << probe_unplugged << "\n";
	// std::cout << "\t" << "pulse_beep = " << pulse_beep << "\n";
	// std::cout << "\t" << "bar_graph = " << unsigned(bar_graph) << "\n";
	// std::cout << "\t" << "finger_out = " << no_finger << "\n";
	// std::cout << "\t" << "waveform_point = " << unsigned(pleth) << "\n";
	// std::cout << "\t" << "searching_pulse = " << pulse_search << "\n";
	// std::cout << "\t" << "pulse_rate = " << unsigned(pulse_rate) << "\n";
	// std::cout << "\t" << "spo2 = " << unsigned(spo2) << "\n";
	// std::cout << std::endl;

	uint32_t cur_ticks = utki::get_ticks_ms();
	uint16_t delta_time = uint16_t(cur_ticks - this->last_ticks);
	this->last_ticks = cur_ticks;

	using std::min;

	constexpr uint8_t max_signal_strength = 8;

	this->push(spo2_measurement{
		.signal_strength = min(signal_strength, max_signal_strength),
		.pulse_beat = pulse_beep,
		.finger_out = no_finger,
		.waveform_point = float(pleth),
		.pulse_rate = pulse_rate,
		.spo2 = float(spo2),
		.perfusion_index = 0,
		.delta_time_ms = delta_time
	});
}
