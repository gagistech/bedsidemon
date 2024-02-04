#pragma once

#include "serial_port_thread.hpp"

namespace bedsidemon {

class contec_cms50d_plus : private serial_port_thread
{
    enum class state{
        disconnected,
        idle,
        read_packet_high_bits,
        read_packet,

        enum_size
    };

    state state_v = state::idle;

    enum class packet_type{
        live_data,

        enum_size
    };

    struct packet{
        uint8_t high_bits;
        packet_type type;
        size_t num_bytes_to_read;
        std::vector<uint8_t> buffer;
    } packet_v;

    bool is_sending = false;
public:
	contec_cms50d_plus(std::string_view port_filename);

private:
	void on_data_received(utki::span<const uint8_t> data) override;
	void on_data_sent() override;
	void on_port_closed() override;

    void request_live_data();

    void feed(uint8_t byte);
    void handle_packet_type_byte(uint8_t byte);
    void handle_packet_byte(uint8_t byte);

    void apply_packet_high_bits();
    void handle_packet();
};

} // namespace bedsidemon
