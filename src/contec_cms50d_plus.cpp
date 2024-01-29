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

#include "contec_cms50d_plus.hpp"

using namespace std::string_view_literals;

using namespace bedsidemon;

constexpr auto serial_port_baud_rate = 115200;

contec_cms50d_plus::contec_cms50d_plus() :
	nitki::loop_thread(1),
	port("/dev/ttyUSB0"sv, serial_port_baud_rate)
{
	this->wait_set.add(this->port, opros::ready::read, &this->port);
	this->start();
}

contec_cms50d_plus::~contec_cms50d_plus()
{
	this->quit();
	this->join();
	this->wait_set.remove(this->port);
}

std::optional<uint32_t> contec_cms50d_plus::on_loop()
{
	auto triggered = this->wait_set.get_triggered();

	std::cout << "loop" << std::endl;
	return {};
}