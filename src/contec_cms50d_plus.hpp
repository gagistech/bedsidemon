#pragma once

#include "serial_port_thread.hpp"

namespace bedsidemon {

class contec_cms50d_plus : private serial_port_thread
{
public:
	contec_cms50d_plus(std::string_view port_filename);

private:
	void on_data_received(utki::span<const uint8_t> data) override;
	void on_data_sent() override;
	void on_port_closed() override;
};

} // namespace bedsidemon
