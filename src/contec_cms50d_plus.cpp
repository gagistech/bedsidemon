#include "contec_cms50d_plus.hpp"

using namespace bedsidemon;

namespace {
constexpr auto serial_port_baud_rate = baud_rate::baud_115200;
} // namespace

contec_cms50d_plus::contec_cms50d_plus(std::string_view port_filename) :
	serial_port_thread(port_filename, serial_port_baud_rate)
{
    this->send({0x7d, 0x81, 0xa1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80});
}

void contec_cms50d_plus::on_data_received(utki::span<const uint8_t> data){
    // TODO:
}

void contec_cms50d_plus::on_data_sent(){
    // TODO:
}

void contec_cms50d_plus::on_port_closed(){
    // TODO:
}
