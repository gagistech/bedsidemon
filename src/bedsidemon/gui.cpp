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

#include "gui.hpp"

#include <ruis/widgets/button/nine_patch_push_button.hpp>
#include <ruis/widgets/label/image.hpp>
#include <ruis/widgets/label/rectangle.hpp>

#include "style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using ruis::lp;
using namespace ruis::length_literals;

using namespace bedsidemon;

namespace {
namespace m {
using namespace ruis::make;
} // namespace m
} // namespace

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_buttons(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return {
		m::nine_patch_push_button(c,
			{
				.widget_params = {
					.lp = {
						.dims = {lp::min, lp::fill}
					}
				},
			  	.nine_patch_button_params = {
					.unpressed_nine_patch = c.get().loader.load<ruis::res::nine_patch>("ruis_npt_button_normal"sv),
              		.pressed_nine_patch = c.get().loader.load<ruis::res::nine_patch>("ruis_npt_button_pressed"sv)
				}
			},
			{
				m::image(c,
					{
						.widget_params = {
							.lp = {
								.dims = {lp::min, lp::fill}
							}
						},
				 		.image_params = {
							.img = c.get().loader.load<ruis::res::image>("img_home"sv),
							.keep_aspect_ratio = true
						}
				 	}
				)
			}
		),
		m::rectangle(c,
			{
				.widget_params = {
					.lp = {
						.dims = {lp::fill, lp::fill},
						.weight = 1
					}
				},
			  	.color_params = {
              		.color = 0xff008080 // NOLINT
              	}
			}
		)
	};
	// clang-format on
}
} // namespace

utki::shared_ref<ruis::widget> bedsidemon::make_root_widgets(utki::shared_ref<ruis::context> c)
{
	constexpr auto alarms_area_height = 70_pp;
	constexpr auto buttons_area_height = 50_pp;

	return
		// clang-format off
		m::container(c,
			{
				.container_params = {
					.layout = ruis::layout::column
				}
			},
			{
				m::container(c,
					{
						.widget_params = {
							.id = "notification_area"s,
							.lp = {
								.dims = {lp::fill, alarms_area_height}
							}
						}
					}
				),
				m::rectangle(c,
					{
						.widget_params = {
							.lp = {
								.dims = {lp::fill, 1_pp}
							}
						},
						.color_params = {
							.color = style::color_border
						}
					}
				),
				m::container(c,
					{
						.widget_params = {
							.id = "pw_container"s,
							.lp = {
								.dims = {lp::fill, lp::fill},
								.weight = 1
							}
						},
						.container_params = {
							.layout = ruis::layout::column
						}
					},
					{}
				),
				m::container(c,
					{
						.widget_params = {
							.id = "button_area"s,
							.lp = {
								.dims = {lp::fill, buttons_area_height}
							}
						},
						.container_params = {
							.layout = ruis::layout::row
						}
					},
					make_buttons(c)
				)
			}
		);
	// clang-format on
}
