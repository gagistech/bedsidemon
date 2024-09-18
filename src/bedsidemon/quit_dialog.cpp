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

#include "quit_dialog.hpp"

#include <ruis/widget/button/push_button.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/label/text.hpp>

#include "application.hpp"
#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace {
constexpr auto dimension_gap = 30_pp;
constexpr auto dimension_button_width = 100_pp;
constexpr auto dimension_button_height = 40_pp;
} // namespace

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_root_widget_structure(utki::shared_ref<ruis::context> c)
{
	auto make_button = [&](std::string id, std::u32string text) {
		// clang-format off
        return m::push_button(c,
            {
                .layout_params{
                    .dims = {dimension_button_width, dimension_button_height}
                },
                .widget_params{
                    .id = std::move(id)
                }
            },
            {
                m::text(c,
                    {},
                    std::move(text)
                )
            }
        );
		// clang-format on
	};

	// clang-format off
    return {
        m::margins(c,
            {
                .container_params{
                    .layout = ruis::layout::column
                },
                .frame_params{
                    .borders{dimension_gap}
                }
            },
            {
                m::text(c,
                    {
                        .text_params{
                            .font_size = 20_pp
                        }
                    },
                    U"Quit program?"s
                ),
                m::gap(c,
                    {
                        .layout_params{
                            .dims = {1_px, dimension_gap}
                        }
                    }
                ),
                m::row(c,
                    {},
                    {
                        make_button("yes_button"s, U"Yes"s),
                        m::gap(c,
                            {
                                .layout_params{
                                    .dims = {dimension_gap, 1_px}
                                }
                            }
                        ),
                        make_button("no_button"s, U"No"s),
                    }
                )
            }
        )
    };
	// clang-format on
}
} // namespace

quit_dialog::quit_dialog(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context), //
		{
			.dims{ruis::dim::fill, ruis::dim::fill}
},
		{}
	),
	ruis::dialog(
		this->context, //
		{.container_params{.layout = ruis::layout::pile}},
		make_root_widget_structure(this->context)
	)
{
	{
		auto& b = this->get_widget_as<ruis::push_button>("yes_button"sv);
		b.click_handler = [](ruis::push_button& b) {
			bedsidemon::application::inst().quit();
		};
	}

	{
		auto& b = this->get_widget_as<ruis::push_button>("no_button"sv);
		b.click_handler = [this](ruis::push_button& b) {
			this->close();
		};
	}
}
