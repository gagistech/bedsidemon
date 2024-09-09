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

#include <functional>
#include <list>

namespace utki {

template <typename... args_type>
class signal
{
public:
	using callback_type = std::function<void(args_type...)>;

private:
	std::list<callback_type> callbacks;

public:
	using connection = typename decltype(callbacks)::iterator;

	connection connect(callback_type callback)
	{
		this->callbacks.push_back(std::move(callback));
		return std::prev(this->callbacks.end());
	}

	void disconnect(connection conn)
	{
		this->callbacks.erase(conn);
	}

	void emit(args_type... a)
	{
		for (const auto& c : this->callbacks) {
			c(a...);
		}
	}

	size_t size() const noexcept
	{
		return this->callbacks.size();
	}

	bool empty() const noexcept
	{
		return this->size() == 0;
	}
};

} // namespace utki
