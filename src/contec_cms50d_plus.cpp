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

#include <utki/util.hpp>

using namespace std::string_view_literals;

using namespace bedsidemon;

constexpr auto serial_port_baud_rate = 115200;

contec_cms50d_plus::contec_cms50d_plus() :
	nitki::loop_thread(1),
	port("/dev/ttyUSB0"sv, serial_port_baud_rate)
{
	this->wait_set.add(this->port, opros::ready::read, &this->port);
	this->start();

    this->send({0x7d, 0x81, 0xa1, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80});
}

contec_cms50d_plus::~contec_cms50d_plus()
{
	this->quit();
	this->join();
	this->wait_set.remove(this->port);
}

void contec_cms50d_plus::send(std::vector<uint8_t> data){
    ASSERT(this->send_buffer.empty())
    
    this->send_buffer = std::move(data);

    this->wait_set.change(this->port, opros::ready::read | opros::ready::write, &this->port);
}

std::optional<uint32_t> contec_cms50d_plus::on_loop()
{
	auto triggered = this->wait_set.get_triggered();

    std::cout << "loop: triggered.size() = " << triggered.size() << std::endl;

    for(const auto& t : triggered){
        if(t.user_data == &this->port){
            if(t.flags.get(opros::ready::write)){
                auto num_sent = this->port.send(this->send_buffer);
                ASSERT(num_sent <= this->send_buffer.size())
                this->send_buffer.erase(this->send_buffer.begin(), utki::next(this->send_buffer.begin(), num_sent));
                if(this->send_buffer.empty()){
                    this->wait_set.change(this->port, opros::ready::read, &this->port);
                }
            }
            if(t.flags.get(opros::ready::read)){
                constexpr auto receive_buffer_size = 0x100;
                std::array<uint8_t, receive_buffer_size> buffer;
                auto num_received = this->port.receive(buffer);
                ASSERT(num_received <= buffer.size())

#ifdef DEBUG
                std::cout << "received = ";
                for(const auto& c : buffer){
                    std::cout << c;
                }
                std::cout << std::endl;
#endif

                this->recieve_buffer.insert(this->recieve_buffer.end(), buffer.begin(), utki::next(buffer.begin(), num_received));
            }
        }
    }

	return {};
}
