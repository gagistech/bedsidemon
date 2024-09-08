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

#include "settings_menu.hpp"

#include <ratio>

#include <ruis/widget/button/selection_box.hpp>
#include <ruis/widget/group/margins.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/text.hpp>

#include "style.hpp"

using namespace ruis::length_literals;

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace bedsidemon;

namespace m {
using namespace ruis::make;
} // namespace m

namespace {
constexpr const std::array<uint32_t, 3> sweep_speeds_um_per_sec = {
	12500, //
	25000,
	50000
};
} // namespace

namespace {
class selection_box_provider : public ruis::selection_box::provider
{
public:
	selection_box_provider() {}

	size_t count() const noexcept override
	{
		return sweep_speeds_um_per_sec.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		ASSERT(index < sweep_speeds_um_per_sec.size())

		auto speed_um_per_sec = *std::next(sweep_speeds_um_per_sec.begin(), index);

		float speed_mm_per_sec = float(speed_um_per_sec) / float(std::kilo::num);

		auto& c = this->get_selection_box()->context;

		// clang-format off
		return m::margins(c,
			{
				.container_params = {
					.layout = ruis::layout::pile
				},
				.frame_params = {
					.borders = {10_pp}
				}
			},
			{
				m::text(c,
					{
						.text_params{
							.font_size = style::font_size_setting
						}
					},
					utki::to_utf32(utki::cat(speed_mm_per_sec, " mm/s"sv))
				)
			}
		);
		// clang-format on
	}
};
} // namespace

namespace {
std::vector<utki::shared_ref<ruis::widget>> make_menu_contents(utki::shared_ref<ruis::context> c)
{
	// clang-format off
	return {
		m::text(c,
			{
				.layout_params = {
					.align = {ruis::align::front, ruis::align::center}
				},
                .text_params = {
                    .font_size = style::font_size_setting
                }
			},
			U"Sweep speed:"s
		),
		m::gap(c,
			{
				.layout_params = {
					.dims{0_px, style::gap_size_setting_label_value}
				}
			}
		),
		m::selection_box(c,
			{
				.layout_params = {
					.dims = {200_pp, ruis::dim::min},
					.align = {ruis::align::front, ruis::align::center}
				},
				.selection_params = {
					.provider = std::make_shared<selection_box_provider>()
				}
			}
		)
	};
	// clang-format on
}
} // namespace

settings_menu::settings_menu(utki::shared_ref<ruis::context> context) :
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
			m::text(this->context,
				{
					.layout_params = {
						.align = {ruis::align::front, ruis::align::center}
					},
					.text_params = {
						.font_size = style::font_size_menu_title
					}
				},
				U"Settings"s
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
				make_menu_contents(this->context)
			)
		}
	)
// clang-format on
{}
