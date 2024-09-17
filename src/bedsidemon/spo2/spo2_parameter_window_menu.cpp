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

#include "spo2_parameter_window_menu.hpp"

#include "spo2_parameter_window.hpp"

using namespace std::string_literals;

using namespace bedsidemon;

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_menu_contents(utki::shared_ref<ruis::context> c)
{
	// clang-format off
    return {
        // TODO:
    };
	// clang-format on
}
} // namespace

spo2_parameter_window_menu::spo2_parameter_window_menu(
	utki::shared_ref<ruis::context> context, //
	std::weak_ptr<spo2_parameter_window> spo2_pw
) :
	// clang-format off
    ruis::widget(
        std::move(context),
        {
            .dims{ruis::dim::fill, ruis::dim::fill}
        },
        {}
    ),
    menu(
        this->context, //
        U"SpO2"s,
        make_menu_contents(this->context)
    ),
    spo2_pw(std::move(spo2_pw))
// clang-format on
{}

void spo2_parameter_window_menu::on_close()
{
	auto pw = this->spo2_pw.lock();
	if (!pw) {
		return;
	}

	pw->pw_menu.reset();
}
