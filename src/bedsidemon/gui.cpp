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

#include <ruis/widget/button/base/push_button.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/label/rectangle.hpp>

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
	auto make_button = [&c](std::string_view icon_res_id){
		return m::push_button(c,
			{
				.layout_params = {
					.dims = {lp::min, lp::fill}
				}
			},
			{
				m::image(c,
					{
						.layout_params = {
							.dims = {lp::min, lp::fill}
						},
				 		.image_params = {
							.img = c.get().loader.load<ruis::res::image>(icon_res_id),
							.keep_aspect_ratio = true
						}
				 	}
				)
			}
		);
	};

	// clang-format off
	return {
		make_button("img_home"sv),
		make_button("img_cog"sv),
		m::rectangle(c,
			{
				.layout_params = {
					.dims = {lp::fill, lp::fill},
					.weight = 1
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
						.layout_params = {
							.dims = {lp::fill, alarms_area_height}
						},
						.widget_params = {
							.id = "notification_area"s,
						}
					}
				),
				m::rectangle(c,
					{
						.layout_params = {
							.dims = {lp::fill, 1_pp}
						},
						.color_params = {
							.color = style::color_border
						}
					}
				),
				m::container(c,
					{
						.layout_params = {
							.dims = {lp::fill, lp::fill},
							.weight = 1
						},
						.widget_params = {
							.id = "pw_container"s
						},
						.container_params = {
							.layout = ruis::layout::column
						}
					},
					{}	
				),
				m::container(c,
					{
						.layout_params = {
							.dims = {lp::fill, buttons_area_height}
						},
						.widget_params = {
							.id = "button_area"s
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
