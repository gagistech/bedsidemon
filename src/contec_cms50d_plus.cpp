#include "contec_cms50d_plus.hpp"

using namespace bedsidemon;

namespace {
constexpr auto serial_port_baud_rate = baud_rate::baud_115200;
} // namespace

contec_cms50d_plus::contec_cms50d_plus(std::string_view port_filename) :
	serial_port_thread(port_filename, serial_port_baud_rate)
{}
