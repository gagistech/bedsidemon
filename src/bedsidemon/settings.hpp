/*
bedsidemon - Bedside monitor example GUI project

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

#include <tml/tree.hpp>
#include <utki/signal.hpp>
#include <utki/singleton.hpp>

namespace bedsidemon {

struct settings {
	constexpr static const auto default_sweep_speed_um_per_sec = 25000;
	uint32_t sweep_speed_um_per_sec = default_sweep_speed_um_per_sec;

	static const std::array<std::pair<std::string_view, std::u32string_view>, 3> language_id_to_name_mapping;
	size_t cur_language_index = 0;
};

class settings_storage : public utki::singleton<settings_storage>
{
	std::string filename;

	settings settings_v;

	static settings read(std::string_view filename);
	void write();

public:
	utki::signal<const settings&> settings_changed_signal;

	settings_storage();
	~settings_storage() override = default;

	settings_storage(const settings_storage&) = delete;
	settings_storage& operator=(const settings_storage&) = delete;

	settings_storage(settings_storage&&) = delete;
	settings_storage& operator=(settings_storage&&) = delete;

	const settings& get() const noexcept
	{
		return this->settings_v;
	}

	void set(const settings& s);
};

} // namespace bedsidemon
