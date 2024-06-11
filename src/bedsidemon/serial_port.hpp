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

#pragma once

#include <opros/waitable.hpp>
#include <utki/span.hpp>

namespace bedsidemon {

enum class baud_rate {
	baud_50,
	baud_75,
	baud_110,
	baud_134,
	baud_150,
	baud_200,
	baud_300,
	baud_600,
	baud_1200,
	baud_1800,
	baud_2400,
	baud_4800,
	baud_9600,
	baud_19200,
	baud_38400,
	baud_57600,
	baud_115200,
	baud_230400,
	baud_460800,
	baud_500000,
	baud_576000,
	baud_921600,
	baud_1000000,
	baud_1152000,
	baud_1500000,
	baud_2000000,

	enum_size
};

class serial_port : public opros::waitable
{
public:
	serial_port(std::string_view port_filename, baud_rate baud_rate);

	serial_port(const serial_port&) = delete;
	serial_port& operator=(const serial_port&) = delete;

	serial_port(serial_port&&) = delete;
	serial_port& operator=(serial_port&&) = delete;

	~serial_port();

	size_t send(utki::span<const uint8_t> data);

	// return number of bytes received.
	// return 0 in case port is closed/disconnected.
	size_t receive(utki::span<uint8_t> buffer);

	void close();
};

} // namespace bedsidemon
