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

#include "about_menu.hpp"

#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/text.hpp>

#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

extern const char* const program_version;

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_contents(utki::shared_ref<ruis::context> c)
{
	constexpr auto font_size_program_title = 20_pp;
	constexpr auto font_size = 16_pp;
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
            c.get().localization.get("about_menu:program_title")
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
            std::u32string(c.get().localization.get("about_menu:version").string()).append(U": "sv).append(utki::to_utf32(program_version))
        ),
        m::gap(c,
            {
                .layout_params{
                    .dims = {0_px, gap_paragraph}
                }
            }
        ),
        m::row(c,
            {},
            {
                m::text(c,
                    {
                        .text_params{
                            .font_size = font_size
                        }
                    },
                    c.get().localization.get("about_menu:powered_by")
                ),
                m::text(c,
                    {
                        .color_params{
                            .color = 0xff8080ff // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
                        },
                        .text_params{
                            .font_size = font_size
                        }
                    },
                    U" ruis"s
                ),
                m::text(c,
                    {
                        .text_params{
                            .font_size = font_size
                        }
                    },
                    U" GUI"s
                )
            }
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
            std::u32string(c.get().localization.get("about_menu:copyright").string()).append(U" © 2024 Gagistech Oy <gagistechoy@gmail.com>"sv)
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
		this->context.get().localization.get("about_menu_title"),
		make_contents(this->context)
	)
{}
