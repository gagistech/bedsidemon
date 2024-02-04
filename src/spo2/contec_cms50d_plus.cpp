#include "contec_cms50d_plus.hpp"

#include <utki/util.hpp>

using namespace bedsidemon;

// see doc/contec_cms50x for communication protocol description

namespace {
constexpr auto serial_port_baud_rate = baud_rate::baud_115200;
} // namespace

contec_cms50d_plus::contec_cms50d_plus(std::string_view port_filename) :
	serial_port_thread(port_filename, serial_port_baud_rate)
{
	this->request_live_data();
}

void contec_cms50d_plus::on_data_received(utki::span<const uint8_t> data)
{
	// std::cout << "data received" << std::endl;
	
	for(const auto& b : data){
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

void contec_cms50d_plus::request_live_data(){
	ASSERT(!this->is_sending)
	this->send({0x7d, 0x81, 0xa1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80});
	this->is_sending = true;
}

void contec_cms50d_plus::feed(uint8_t byte){
	switch(this->state_v){
		case state::disconnected:
			// ignore any data, it is not supposed to come from disconnected port
			break;
		case state::idle:
			if(byte & 0x80){
				// not a packet type byte,
				// need to wait for next packet start,
				// ignore
				break;
			}
			this->handle_packet_type_byte(byte);
			break;
		case state::read_packet_high_bits:
			if(!(byte & 0x80)){
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
			if(this->packet_v.num_bytes_to_read == 0){
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

namespace{
constexpr auto live_data_packet_type = 0x01;
}

void contec_cms50d_plus::handle_packet_type_byte(uint8_t byte){
	ASSERT(this->state_v == state::idle)
	switch(byte){
		case live_data_packet_type:
			this->packet_v.type = packet_type::live_data;
			this->packet_v.num_bytes_to_read = 7;
			std::cout << "live data packet" << std::endl;
			break;
		default:
			std::cout << "packet type = " << byte << std::endl;
			// unknown packet, ignore
			// remain in wait_response state
			return;
	}
	this->state_v = state::read_packet_high_bits;
}

void contec_cms50d_plus::apply_packet_high_bits(){
	ASSERT(this->packet_v.buffer.size() <= utki::byte_bits - 1)
	for(auto& b : this->packet_v.buffer){
		b &= (utki::byte_mask >> 1);
		b |= ((this->packet_v.high_bits & 0x01) << (utki::byte_bits - 1));
		this->packet_v.high_bits >>= 1;
	}
}

struct live_data
{
	enum class status{
		finger_out,
		searching
	};

	uint8_t signal_strength;

	bool pulse_beep;

	uint8_t waveform_point;

	uint8_t pulse_rate; // 0xff = invalid

	uint8_t spo2; // oxygenation, %, >100 = invalid
};

void contec_cms50d_plus::handle_packet(){
	// std::cout << "handle packet, high bits = " << std::hex << "0x" << unsigned(this->packet_v.high_bits) << std::endl;
	// for(const auto& b : this->packet_v.buffer){
	// 	std::cout << "0x" << std::hex << unsigned(b) << " ";
	// }
	// std::cout << std::endl;

	if(this->packet_v.type == packet_type::live_data){
		live_data data;

		data.signal_strength = this->packet_v.buffer[0] & utki::lower_nibble_mask;

		data.pulse_beep = (this->packet_v.buffer[0] & 0b01000000) != 0;

		data.waveform_point = this->packet_v.buffer[1] & 0x7f;

		data.pulse_rate = this->packet_v.buffer[3];
		data.spo2 = this->packet_v.buffer[4];

		std::cout << "signal_strength = " << unsigned(data.signal_strength) << "\n";
		std::cout << "\t" << "pulse_beep = " << data.pulse_beep << "\n";
		std::cout << "\t" << "waveform_point = " << unsigned(data.waveform_point) << "\n";
		std::cout << "\t" << "pulse_rate = " << unsigned(data.pulse_rate) << "\n";
		std::cout << "\t" << "spo2 = " << unsigned(data.spo2) << "\n";

		std::cout << std::endl;
	}
}
