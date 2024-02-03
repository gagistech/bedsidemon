/*
bedsidemon - Bed-side monitor example GUI project

Copyright (C) Ivan Gagis  <igagis@gmail.com>

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

#include <nitki/loop_thread.hpp>

#include "serial_port.hpp"

namespace bedsidemon {

class serial_port_thread : private nitki::loop_thread
{
	serial_port port;

	std::mutex send_buffer_mutex;

	std::vector<uint8_t> send_buffer;

public:
	serial_port_thread(std::string_view port_filename, baud_rate speed);

	serial_port_thread(const serial_port_thread&) = delete;
	serial_port_thread& operator=(const serial_port_thread&) = delete;

	serial_port_thread(serial_port_thread&&) = delete;
	serial_port_thread& operator=(serial_port_thread&&) = delete;

	~serial_port_thread() override;

	void send(std::vector<uint8_t> data);

	// these callbacks are invoked from within this thread
	virtual void on_data_sent() = 0;
	virtual void on_data_received(utki::span<const uint8_t> data) = 0;
	virtual void on_port_closed() = 0;

private:
	std::optional<uint32_t> on_loop() override;
	void on_quit()override;
};

} // namespace bedsidemon
