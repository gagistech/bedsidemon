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

#include "settings_menu.hpp"

#include <algorithm>
#include <ratio>

#include <ruis/widget/button/selection_box.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/label/text.hpp>
#include <utki/debug.hpp>

#include "application.hpp"
#include "style.hpp"

using namespace ruis::length_literals;

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace bedsidemon;

namespace {
constexpr const std::array<uint32_t, 3> sweep_speeds_um_per_sec = {
	12500, //
	25000,
	50000
};
} // namespace

namespace {
class sweep_speed_selection_box_provider : public ruis::list_provider
{
public:
	sweep_speed_selection_box_provider(utki::shared_ref<ruis::context> context) :
		list_provider(std::move(context))
	{}

	size_t count() const noexcept override
	{
		return sweep_speeds_um_per_sec.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		ASSERT(index < sweep_speeds_um_per_sec.size())

		auto speed_um_per_sec = *utki::next(sweep_speeds_um_per_sec.begin(), index);

		float speed_mm_per_sec = float(speed_um_per_sec) / float(std::kilo::num);

		auto& c = this->context;

		// clang-format off
		return m::margins(c,
			{
				.container_params = {
					.layout = ruis::layout::pile
				},
				.frame_params = {
					.borders = {10_pp} // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
				}
			},
			{
				m::text(c,
					{
						.text_params{
							.font_size = style::font_size_setting
						}
					},
					c.get().localization.get().get("settings_menu:sweep_speed_value").format({utki::to_utf32(utki::cat(speed_mm_per_sec))})
				)
			}
		);
		// clang-format on
	}
};
} // namespace

namespace {
class language_selection_box_provider : public ruis::list_provider
{
public:
	language_selection_box_provider(utki::shared_ref<ruis::context> context) :
		list_provider(std::move(context))
	{}

	size_t count() const noexcept override
	{
		return settings::language_id_to_name_mapping.size();
	}

	utki::shared_ref<ruis::widget> get_widget(size_t index) override
	{
		const auto& lang_mapping = settings::language_id_to_name_mapping;

		ASSERT(index < lang_mapping.size())

		auto lang_name = utki::next(lang_mapping.begin(), index)->second;

		auto& c = this->context;

		// clang-format off
		return m::margins(c,
			{
				.container_params = {
					.layout = ruis::layout::pile
				},
				.frame_params = {
					.borders = {10_pp} // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
				}
			},
			{
				m::text(c,
					{
						.text_params{
							.font_size = style::font_size_setting
						}
					},
					std::u32string(lang_name)
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
	auto language_selection_box = m::selection_box(c,
		{
			.layout_params = {
				.dims = {200_pp, ruis::dim::min}, // NOLINT(cppcoreguidelines-avoid-magic-numbers, "TODO: fix")
				.align = {ruis::align::front, ruis::align::center}
			},
			.providable_params = {
				.provider = std::make_shared<language_selection_box_provider>(c)
			}
		}
	);
	// clang-format on

	{
		language_selection_box.get().selection_handler = [](ruis::selection_box& sb) {
			auto sel = sb.get_selection();

			// save the language to settings storage
			{
				auto& ss = settings_storage::inst();
				auto s = ss.get();

				ASSERT(sel < settings::language_id_to_name_mapping.size())
				s.cur_language_index = sel;

				ss.set(s);
			}

			// reload the ui
			{
				sb.context.get().post_to_ui_thread([sel]() {
					auto& app = bedsidemon::application::inst();
					app.load_language(sel);
				});
			}
		};

		auto& ss = settings_storage::inst();
		const auto& s = ss.get();
		ASSERT(s.cur_language_index < settings::language_id_to_name_mapping.size())

		language_selection_box.get().set_selection(s.cur_language_index);
	}

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
			c.get().localization.get().get("settings_menu:sweep_speed_title")
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
					.id = "sweep_speed_selection_box"s
				},
				.providable_params = {
					.provider = std::make_shared<sweep_speed_selection_box_provider>(c)
				}
			}
		),
		m::gap(c,
			{
				.layout_params = {
					.dims{0_px, style::gap_size_between_settings}
				}
			}
		),
		m::text(c,
			{
				.layout_params = {
					.align = {ruis::align::front, ruis::align::center}
				},
                .text_params = {
                    .font_size = style::font_size_setting
                }
			},
			c.get().localization.get().get("settings_menu:language_title")
		),
		m::gap(c,
			{
				.layout_params = {
					.dims{0_px, style::gap_size_setting_label_value}
				}
			}
		),
		std::move(language_selection_box)
	};
	// clang-format on
}
} // namespace

settings_menu::settings_menu(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context),
		{
			.dims = {ruis::dim::fill, ruis::dim::fill}  //
},
		{}
	),
	menu(
		this->context, //
		this->context.get().localization.get().get("settings_menu:title"),
		make_menu_contents(this->context)
	)
{
	{
		auto& sb = this->get_widget_as<ruis::selection_box>("sweep_speed_selection_box"sv);
		sb.selection_handler = [](ruis::selection_box& sb) {
			ASSERT(sb.get_selection() < sweep_speeds_um_per_sec.size())
			auto speed = sweep_speeds_um_per_sec[sb.get_selection()];

			auto& ss = settings_storage::inst();

			auto s = ss.get();
			s.sweep_speed_um_per_sec = speed;
			ss.set(s);
		};

		auto& ss = settings_storage::inst();
		const auto& s = ss.get();
		auto i = std::find(
			sweep_speeds_um_per_sec.begin(), //
			sweep_speeds_um_per_sec.end(),
			s.sweep_speed_um_per_sec
		);
		if (i != sweep_speeds_um_per_sec.end()) {
			sb.set_selection(std::distance(sweep_speeds_um_per_sec.begin(), i));
		} else {
			// TODO: settings file contains invalid value, handle it gracefully
		}
	}
}
