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

#include "about_menu.hpp"

#include <ruis/widget/label/text.hpp>
#include <ruis/widget/label/gap.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_contents(utki::shared_ref<ruis::context> c)
{
    constexpr auto font_size_program_title = 20_pp;
    constexpr auto color_program_title = 0xff00ffff;

    constexpr auto gap_paragraph = 20_pp;

	// clang-format off
	return {
        m::text(c,
            {
                .color_params{
                    .color = color_program_title
                },
                .text_params{
                    .font_size = font_size_program_title
                }
            },
            U"Bedside Patient Monitor DEMO program"s
        ),
        m::gap(c,
            {
                .layout_params{
                    .dims = {0_px, gap_paragraph}
                }
            }
        ),
        m::text(c,
            {},
            U"Version: TODO"s
        ),
        m::gap(c,
            {
                .layout_params{
                    .dims = {0_px, gap_paragraph}
                }
            }
        ),
        m::text(c,
            {},
            U"Copyright (C) 2024 Gagistech Oy <gagistechoy@gmail.com>"s
        )
    };
	// clang-format on
}
} // namespace

about_menu::about_menu(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context),
		{
			.dims = {ruis::dim::fill, ruis::dim::fill}
},
		{}
	),
	menu(
		this->context, //
		U"About"s,
		make_contents(this->context)
	)
{}
