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

	std::vector<uint8_t> send_buffer;
	std::vector<uint8_t> recieve_buffer;

public:
	serial_port_thread(std::string_view port_filename, baud_rate speed);

	serial_port_thread(const serial_port_thread&) = delete;
	serial_port_thread& operator=(const serial_port_thread&) = delete;

	serial_port_thread(serial_port_thread&&) = delete;
	serial_port_thread& operator=(serial_port_thread&&) = delete;

	~serial_port_thread() override;

private:
	std::optional<uint32_t> on_loop() override;

	void send(std::vector<uint8_t> data);
};

} // namespace bedsidemon
