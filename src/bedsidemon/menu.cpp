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

#include "menu.hpp"

#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/label/text.hpp>

#include "application.hpp"
#include "rectangle_push_button.hpp"
#include "style.hpp"

using namespace std::string_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace {
constexpr auto size_close_button = 40_pp;
} // namespace

menu::menu(
	utki::shared_ref<ruis::context> context, //
	ruis::string title,
	utki::span<const utki::shared_ref<ruis::widget>> contents
) :
	// clang-format off
	ruis::widget(
		std::move(context),
		{
			.dims = {ruis::dim::fill, ruis::dim::fill}
		},
		{}
	),
	ruis::nine_patch(
		this->context,
		{
			.container_params = {
				.layout = ruis::layout::column
			},
			.nine_patch_params = {
				.nine_patch = this->context.get().loader.load<ruis::res::nine_patch>("ruis_npt_window_bg")
			}
		},
		{
			m::row(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::min},
						.align{ruis::align::front, ruis::align::center}
					}
				},
				{
					m::text(this->context,
						{
							.layout_params = {
								.align = {ruis::align::front, ruis::align::center}
							},
							.text_params = {
								.font_size = style::font_size_menu_title
							}
						},
						std::move(title)
					),
					m::gap(this->context,
						{
							.layout_params = {
								.weight = 1
							}
						}
					),
					m::rectangle_push_button(this->context,
						{
							.layout_params{
								.dims{size_close_button, size_close_button}
							},
							.widget_params{
								.id = "close_button"s
							},
							.container_params{
								.layout = ruis::layout::pile
							}
						},
						{
							m::image(this->context,
								{
									.layout_params{
										.dims{ruis::dim::fill, ruis::dim::fill}
									},
									.image_params{
										.img = this->context.get().loader.load<ruis::res::image>("img_close")
									}
								}
							)
						}
					)
				}
			),
			m::margins(this->context,
				{
					.layout_params{
						.dims{ruis::dim::fill, ruis::dim::fill},
						.weight = 1
					},
					.container_params{
						.layout = ruis::layout::column
					},
					.frame_params{
						.borders = {style::menu_padding}
					}
				},
				contents
			)
		}
	)
// clang-format on
{
	this->get_widget_as<ruis::push_button>("close_button").click_handler = [](ruis::push_button& b) {
		b.context.get().post_to_ui_thread([]() {
			bedsidemon::application::inst().close_menu();
		});
	};
}
