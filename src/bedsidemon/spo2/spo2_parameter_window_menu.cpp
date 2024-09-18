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

#include <algorithm>

#include <ruis/widget/button/selection_box.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/label/rectangle.hpp>

#include "../style.hpp"

#include "spo2_parameter_window.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace {
class selection_box_provider : public ruis::selection_box::provider
{
public:
	selection_box_provider() = default;

	size_t count() const noexcept override
	{
		return spo2_parameter_window::possible_colors.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		auto& c = this->get_selection_box()->context;

		// clang-format off
		return m::margins(c,
			{
				.container_params = {
					.layout = ruis::layout::pile
				},
				.frame_params = {
					.borders = {3_pp} // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
				}
			},
			{
				m::rectangle(c,
					{
                        .layout_params{
                            .dims{40_pp, 30_pp} // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
                        },
						.color_params{
							.color = spo2_parameter_window::possible_colors.at(index)
						}
					}
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
			U"Color:"s
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
					.dims = {200_pp, ruis::dim::min}, // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
					.align = {ruis::align::front, ruis::align::center}
				},
				.widget_params = {
					.id = "color_selection_box"s
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
{
	{
		auto& sb = this->get_widget_as<ruis::selection_box>("color_selection_box"sv);

		if (auto pw = this->spo2_pw.lock()) {
			const auto& colors = spo2_parameter_window::possible_colors;
			auto i = std::find(
				colors.begin(), //
				colors.end(),
				pw->get_color()
			);
			sb.set_selection(std::distance(colors.begin(), i));
		}

		sb.selection_handler = [this](ruis::selection_box& sb) {
			auto pw = this->spo2_pw.lock();
			if (!pw) {
				return;
			}
			pw->set_color(spo2_parameter_window::possible_colors.at(sb.get_selection()));
		};
	}
}

void spo2_parameter_window_menu::on_close()
{
	auto pw = this->spo2_pw.lock();
	if (!pw) {
		return;
	}

	pw->pw_menu.reset();
}
