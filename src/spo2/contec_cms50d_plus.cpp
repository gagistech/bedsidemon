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

#include "contec_cms50d_plus.hpp"

#include <utki/time.hpp>
#include <utki/util.hpp>

using namespace bedsidemon;

// see doc/contec_cms50x for communication protocol description

namespace {
constexpr auto serial_port_baud_rate = baud_rate::baud_115200;
} // namespace

contec_cms50d_plus::contec_cms50d_plus(utki::shared_ref<spo2_parameter_window> pw, std::string_view port_filename) :
	spo2_sensor(std::move(pw)),
	serial_port_thread(port_filename, serial_port_baud_rate)
{
	this->request_live_data(utki::get_ticks_ms());

	this->start();
}

contec_cms50d_plus::~contec_cms50d_plus()
{
	this->quit();
	this->join();
}

void contec_cms50d_plus::on_data_received(utki::span<const uint8_t> data)
{
	// std::cout << "data received" << std::endl;

	for (const auto& b : data) {
		this->feed(b);
	}
}

void contec_cms50d_plus::on_data_sent()
{
	this->is_sending = false;
}

void contec_cms50d_plus::on_port_closed()
{
	this->state_v = state::disconnected;
}

void contec_cms50d_plus::request_live_data(uint32_t cur_ticks)
{
	ASSERT(!this->is_sending)

	this->last_ticks = cur_ticks;
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
	this->send({0x7d, 0x81, 0xa1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80});
	this->is_sending = true;
}

void contec_cms50d_plus::feed(uint8_t byte)
{
	switch (this->state_v) {
		case state::disconnected:
			// ignore any data, it is not supposed to come from disconnected port
			break;
		case state::idle:
			if (byte & utki::bit_7_mask) {
				// not a packet type byte,
				// need to wait for next packet start,
				// ignore
				break;
			}
			this->handle_packet_type_byte(byte);
			break;
		case state::read_packet_high_bits:
			if (!(byte & utki::bit_7_mask)) {
				// not a packet body byte, process it as packet type byte
				this->state_v = state::idle;
				this->handle_packet_type_byte(byte);
				break;
			}
			this->packet_v.high_bits = byte;
			this->state_v = state::read_packet;
			break;
		case state::read_packet:
			this->packet_v.buffer.push_back(byte);
			--this->packet_v.num_bytes_to_read;
			if (this->packet_v.num_bytes_to_read == 0) {
				this->state_v = state::idle;
				this->apply_packet_high_bits();
				this->handle_packet();
				this->packet_v.buffer.clear();
			}
			break;
		case state::enum_size:
			ASSERT(false)
			break;
	}
}

namespace {
constexpr auto live_data_packet_type = 0x01;
constexpr auto live_data_packet_size = 7;
} // namespace

void contec_cms50d_plus::handle_packet_type_byte(uint8_t byte)
{
	ASSERT(this->state_v == state::idle)
	switch (byte) {
		case live_data_packet_type:
			this->packet_v.type = packet_type::live_data;
			this->packet_v.num_bytes_to_read = live_data_packet_size;
			// std::cout << "live data packet" << std::endl;
			break;
		default:
			std::cout << "unknown packet type = " << byte << std::endl;
			// unknown packet, ignore
			// remain in wait_response state
			return;
	}
	this->state_v = state::read_packet_high_bits;
}

void contec_cms50d_plus::apply_packet_high_bits()
{
	ASSERT(this->packet_v.buffer.size() <= utki::byte_bits - 1)
	for (auto& b : this->packet_v.buffer) {
		b &= (utki::byte_mask >> 1);
		b |= ((this->packet_v.high_bits & 0x01) << (utki::byte_bits - 1));
		this->packet_v.high_bits >>= 1;
	}
}

namespace {
struct live_data {
	uint8_t signal_strength;

	bool searching_time_too_long;

	bool pulse_beep;

	bool finger_out;

	bool searching_pulse;

	bool is_pi_data_valid;

	uint8_t waveform_point;

	uint8_t pulse_rate; // 0xff = invalid

	uint8_t spo2; // oxygenation, %, >100 = invalid

	uint16_t pi;
};
} // namespace

void contec_cms50d_plus::handle_packet()
{
	// std::cout << "handle packet, high bits = " << std::hex << "0x" << unsigned(this->packet_v.high_bits) <<
	// std::endl; for(const auto& b : this->packet_v.buffer){ 	std::cout << "0x" << std::hex << unsigned(b) << " ";
	// }
	// std::cout << std::endl;

	if (this->packet_v.type == packet_type::live_data) {
		ASSERT(this->packet_v.buffer.size() == live_data_packet_size)
		live_data data{
			.signal_strength = uint8_t(this->packet_v.buffer[0] & utki::lower_nibble_mask),
			.searching_time_too_long = (this->packet_v.buffer[0] & utki::bit_4_mask) != 0,
			.pulse_beep = (this->packet_v.buffer[0] & utki::bit_6_mask) != 0,
			.finger_out = (this->packet_v.buffer[0] & utki::bit_7_mask) != 0,
			.searching_pulse = (this->packet_v.buffer[1] & utki::bit_7_mask) != 0,
			.is_pi_data_valid = (this->packet_v.buffer[2] & utki::bit_4_mask) == 0,
			.waveform_point = uint8_t(this->packet_v.buffer[1] & (~utki::bit_7_mask)),
			.pulse_rate = this->packet_v.buffer[3],
			.spo2 = this->packet_v.buffer[4],
			.pi = utki::deserialize16le(&this->packet_v.buffer[5]), // NOLINT(cppcoreguidelines-avoid-magic-numbers)
		};

		// std::cout << "signal_strength = " << unsigned(data.signal_strength) << "\n";
		// std::cout << "\t" << "searching_time_too_long = " << data.searching_time_too_long << "\n";
		// std::cout << "\t" << "pulse_beep = " << data.pulse_beep << "\n";
		// std::cout << "\t" << "finger_out = " << data.finger_out << "\n";
		// std::cout << "\t" << "waveform_point = " << unsigned(data.waveform_point) << "\n";
		// std::cout << "\t" << "searching_pulse = " << data.searching_pulse << "\n";
		// std::cout << "\t" << "is_pi_data_valid = " << data.is_pi_data_valid << "\n";
		// std::cout << "\t" << "pulse_rate = " << unsigned(data.pulse_rate) << "\n";
		// std::cout << "\t" << "spo2 = " << unsigned(data.spo2) << "\n";
		// std::cout << "\t" << "pi = " << unsigned(data.pi) << "\n";
		// std::cout << std::endl;

		uint32_t cur_ticks = utki::get_ticks_ms();
		auto delta_time = uint16_t(cur_ticks - this->last_ticks);
		this->last_ticks = cur_ticks;

		using std::min;
		using std::max;

		constexpr auto max_signal_strength = 10;
		constexpr auto min_signal_strength = 4;

		this->push(spo2_measurement{
			.signal_strength = uint8_t(
				(min(max(int(data.signal_strength), min_signal_strength), max_signal_strength) - min_signal_strength) *
				std::centi::den / (max_signal_strength - min_signal_strength)
			),
			.pulse_beat = data.pulse_beep,
			.finger_out = data.finger_out,
			.waveform_point = float(data.waveform_point),
			.pulse_rate = data.pulse_rate,
			.spo2 = data.spo2,
			.perfusion_index = data.pi,
			.delta_time_ms = delta_time
		});

		constexpr auto sample_rate = 60;
		constexpr auto acquisition_time_sec = 30;

		// CMS50D+ has limitation of sending live data for only 30 seconds after it was requested.
		// Workaround this limitation by requesting live data every ~15 seconds, assuming that it sends
		// about 60 live data packets per second.
		++this->num_live_data_packages_received;
		if (this->num_live_data_packages_received > sample_rate * (acquisition_time_sec / 2)) {
			this->num_live_data_packages_received = 0;
			this->request_live_data(cur_ticks);
		}
	}
}
