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
	// std::cout << "handle packet, high bits = " << std::hex << "0x" << unsigned(this->packet_v.high_bits) <<
	// std::endl; for(const auto& b : this->packet_v.buffer){ 	std::cout << "0x" << std::hex << unsigned(b) << " ";
	// }
	// std::cout << std::endl;

	ASSERT(this->packet_v.buffer.size() == bci_protocol_packet_size)

	std::cout << "packet received =";
	for (const auto& b : this->packet_v.buffer) {
		std::cout << " 0x" << std::hex << unsigned(b);
	}
	std::cout << std::endl;

	// if (this->packet_v.type == packet_type::live_data) {
	// 	ASSERT(this->packet_v.buffer.size() == live_data_packet_size)
	// 	live_data data;

	// 	data.signal_strength = this->packet_v.buffer[0] & utki::lower_nibble_mask;
	// 	data.searching_time_too_long = (this->packet_v.buffer[0] & utki::bit_4_mask) != 0;
	// 	data.pulse_beep = (this->packet_v.buffer[0] & utki::bit_6_mask) != 0;
	// 	data.finger_out = (this->packet_v.buffer[0] & utki::bit_7_mask) != 0;

	// 	data.waveform_point = this->packet_v.buffer[1] & (~utki::bit_7_mask);
	// 	data.searching_pulse = (this->packet_v.buffer[1] & utki::bit_7_mask) != 0;
	// 	data.is_pi_data_valid = (this->packet_v.buffer[2] & utki::bit_4_mask) == 0;

	// 	data.pulse_rate = this->packet_v.buffer[3];
	// 	data.spo2 = this->packet_v.buffer[4];
	// 	data.pi = utki::deserialize16le(&this->packet_v.buffer[5]);

	// 	// std::cout << "signal_strength = " << unsigned(data.signal_strength) << "\n";
	// 	// std::cout << "\t" << "searching_time_too_long = " << data.searching_time_too_long << "\n";
	// 	// std::cout << "\t" << "pulse_beep = " << data.pulse_beep << "\n";
	// 	// std::cout << "\t" << "finger_out = " << data.finger_out << "\n";
	// 	// std::cout << "\t" << "waveform_point = " << unsigned(data.waveform_point) << "\n";
	// 	// std::cout << "\t" << "searching_pulse = " << data.searching_pulse << "\n";
	// 	// std::cout << "\t" << "is_pi_data_valid = " << data.is_pi_data_valid << "\n";
	// 	// std::cout << "\t" << "pulse_rate = " << unsigned(data.pulse_rate) << "\n";
	// 	// std::cout << "\t" << "spo2 = " << unsigned(data.spo2) << "\n";
	// 	// std::cout << "\t" << "pi = " << unsigned(data.pi) << "\n";
	// 	// std::cout << std::endl;

	// 	uint32_t cur_ticks = utki::get_ticks_ms();
	// 	uint16_t delta_time = uint16_t(cur_ticks - this->last_ticks);
	// 	this->last_ticks = cur_ticks;

	// 	using std::min;
	// 	using std::max;

	// 	this->push(spo2_measurement{
	// 		.signal_strength = uint8_t(
	// 			(min(max(int(data.signal_strength), 4), 10) - 4) * std::centi::den / 6
	// 		), // value is from [4, 10]
	// 		.pulse_beat = data.pulse_beep,
	// 		.finger_out = data.finger_out,
	// 		.waveform_point = float(data.waveform_point),
	// 		.pulse_rate = data.pulse_rate,
	// 		.spo2 = float(data.spo2),
	// 		.perfusion_index = data.pi,
	// 		.delta_time_ms = delta_time
	// 	});

	// 	// CMS50D+ has limitation of sending live data for only 30 seconds after it was requested.
	// 	// Workaround this limitation by requesting live data every ~15 seconds, assuming that it sends
	// 	// about 60 live data packets per second.
	// 	++this->num_live_data_packages_received;
	// 	if (this->num_live_data_packages_received > 60 * 15) {
	// 		this->num_live_data_packages_received = 0;
	// 		this->request_live_data(cur_ticks);
	// 	}
	// }
}
