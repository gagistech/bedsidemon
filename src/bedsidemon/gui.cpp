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

#include <ruis/widgets/label/rectangle.hpp>

#include "style.hpp"

using namespace std::string_literals;

using namespace bedsidemon;

utki::shared_ref<ruis::widget> bedsidemon::make_root_widgets(utki::shared_ref<ruis::context> c)
{
	namespace m = ruis::make;
	using ruis::lp;
	using namespace ruis::length_literals;

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
								.dims = {lp::fill, 70_pp}
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
								.dims = {lp::fill, 50_pp}
							}
						},
						.container_params = {
							.layout = ruis::layout::pile
						}
					},
					{
						m::rectangle(c,
							{
								.widget_params = {
									.lp = {
										.dims = {lp::fill, lp::fill}
									}
								},
								.color_params = {
									.color = 0xff008080
								}
							}
						)
					}
				)
			}
		);
	// clang-format on
}
