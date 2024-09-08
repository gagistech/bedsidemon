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

#include "serial_port.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <utki/string.hpp>
#include <utki/util.hpp>

using namespace bedsidemon;

namespace {
const std::array<speed_t, size_t(baud_rate::enum_size)> baud_rate_map = {
	B50,     B75,     B110,    B134,    B150,     B200,     B300,     B600,    B1200,
	B1800,   B2400,   B4800,   B9600,   B19200,   B38400,   B57600,   B115200, B230400,
	B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000
};
} // namespace

serial_port::serial_port(std::string_view port_filename, baud_rate baud_rate) :
	opros::waitable([&]() {
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int fd = open(utki::make_string(port_filename).c_str(), O_RDWR | O_NOCTTY);
		if (fd < 0) {
			throw std::runtime_error("serial_port(): could not open serial port");
		}

		utki::scope_exit scope_exit([&]() {
			::close(fd);
		});

		// TODO: move setting port config to the class's public method
		termios newtermios{
			.c_iflag = IGNPAR | IGNBRK, //
			.c_oflag = 0,
			.c_cflag = CBAUD | CS8 | CLOCAL | CREAD,
			.c_lflag = 0
		};
		newtermios.c_cc[VMIN] = 0;
		newtermios.c_cc[VTIME] = 0;

		ASSERT(size_t(baud_rate) < size_t(baud_rate::enum_size))
		// NOLINTNEXTLINE(cppcoreguidelines-init-variables, "false positive")
		speed_t br = baud_rate_map[size_t(baud_rate)];
		cfsetospeed(&newtermios, br);
		cfsetispeed(&newtermios, br);

		if (tcflush(fd, TCIOFLUSH) == -1) {
			throw std::runtime_error("serial_port(): could not flush serial port");
		}
		if (tcsetattr(fd, TCSANOW, &newtermios) == -1) {
			throw std::runtime_error("serial_port(): could not set serial port config");
		}
		scope_exit.release();
		return fd;
	}())
{}

serial_port::~serial_port()
{
	this->close();
}

void serial_port::close()
{
	::close(this->handle);

	// set invalid file descriptor because close() can be called again on closed
	// serail port
	this->handle = -1;
}

size_t serial_port::send(utki::span<const uint8_t> data)
{
	auto res = write(this->handle, data.data(), data.size_bytes());
	if (res < 0) {
		throw std::runtime_error("serial_port::send(): failed");
	}
	return res;
}

size_t serial_port::receive(utki::span<uint8_t> buffer)
{
	auto ret = read(this->handle, buffer.data(), buffer.size_bytes());
	if (ret < 0) {
		throw std::runtime_error("serial_port::receive(): failed");
	}

	return ret;
}
