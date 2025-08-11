/*
bedsidemon - Bedside monitor example GUI project

Copyright (C) 2024-2025 Gagistech Oy <gagistechoy@gmail.com>

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

#include "serial_port_thread.hpp"

#include <utki/debug.hpp>
#include <utki/util.hpp>

using namespace bedsidemon;

serial_port_thread::serial_port_thread(std::string_view port_filename, baud_rate speed) :
	nitki::loop_thread(1),
	port(port_filename, speed)
{
	this->wait_set.add(this->port, opros::ready::read, &this->port);
}

serial_port_thread::~serial_port_thread()
{
	this->wait_set.remove(this->port);
}

void serial_port_thread::on_quit()
{
	this->port.close();
	this->on_port_closed();
}

void serial_port_thread::send(std::vector<uint8_t> data)
{
	std::lock_guard lock_guard(this->send_buffer_mutex);

	if (this->send_buffer.empty()) {
		this->send_buffer = std::move(data);
	} else {
		this->send_buffer.reserve(this->send_buffer.size() + data.size());

		for (auto d : data) {
			this->send_buffer.push_back(d);
		}
	}

	this->wait_set.change(
		this->port, //
		opros::ready::read | opros::ready::write,
		&this->port
	);
}

std::optional<uint32_t> serial_port_thread::on_loop()
{
	auto triggered = this->wait_set.get_triggered();

	// std::cout << "loop: triggered.size() = " << triggered.size() << std::endl;

	for (const auto& t : triggered) {
		if (t.user_data == &this->port) {
			if (t.flags.get(opros::ready::write)) {
				std::lock_guard lock_guard(this->send_buffer_mutex);

				auto num_sent = this->port.send(this->send_buffer);
				ASSERT(num_sent <= this->send_buffer.size())
				this->send_buffer.erase(this->send_buffer.begin(), utki::next(this->send_buffer.begin(), num_sent));
				if (this->send_buffer.empty()) {
					// std::cout << "all sent" << std::endl;
					this->wait_set.change(this->port, opros::ready::read, &this->port);
					this->on_data_sent();
				}
			}
			if (t.flags.get(opros::ready::read)) {
				constexpr auto receive_buffer_size = 0x100;
				std::array<uint8_t, receive_buffer_size> buffer{};
				auto num_received = this->port.receive(buffer);
				ASSERT(num_received <= buffer.size())

				auto received_span = utki::make_span(buffer.data(), num_received);

				// std::cout << "received " << num_received << " bytes: " << std::hex;
				// for (const auto& c : received_span) {
				// 		std::cout << unsigned(c) << " ";
				// }
				// std::cout << std::endl;

				this->on_data_received(received_span);
			}
			if (t.flags.get(opros::ready::error)) {
				utki::log([](auto& o) {
					o << "serial port error" << std::endl;
				});
				this->quit();
			}
		}
	}

	return {};
}
