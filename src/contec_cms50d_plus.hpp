#pragma once

#include "serial_port_thread.hpp"

namespace bedsidemon {

class contec_cms50d_plus : private serial_port_thread
{
public:
	contec_cms50d_plus(std::string_view port_filename);
};

} // namespace bedsidemon
