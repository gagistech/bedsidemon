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

#include "serial_port.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <utki/string.hpp>
#include <utki/util.hpp>

using namespace bedsidemon;

serial_port::serial_port(std::string_view port_filename, unsigned baud_rate) :
	opros::waitable([&]() {
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int fd = open(utki::make_string(port_filename).c_str(), O_RDWR | O_NOCTTY);
		if (fd < 0) {
			throw std::runtime_error("serial_port(): could not open serial port");
		}

		utki::scope_exit scope_exit([&]() {
			close(fd);
		});

		// TODO: move setting port config to the classe's public method
		termios newtermios{};
		newtermios.c_cflag = CBAUD | CS8 | CLOCAL | CREAD;
		newtermios.c_iflag = IGNPAR;
		newtermios.c_oflag = 0;
		newtermios.c_lflag = 0;
		newtermios.c_cc[VMIN] = 1;
		newtermios.c_cc[VTIME] = 0;

		cfsetospeed(&newtermios, baud_rate);
		cfsetispeed(&newtermios, baud_rate);

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
	close(this->handle);
}

void serial_port::send(utki::span<const uint8_t> data)
{
	auto res = write(this->handle, data.data(), data.size_bytes());
	if (res < 0) {
		throw std::runtime_error("serial_port::send(): failed");
	}
}

size_t serial_port::receive(utki::span<uint8_t> buffer)
{
	auto ret = read(this->handle, buffer.data(), buffer.size_bytes());
	if (ret < 0) {
		throw std::runtime_error("serial_port::receive(): failed");
	}

	return ret;
}
