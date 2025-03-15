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

#include "dialog.hpp"

#include <ruis/widget/label/nine_patch.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/proxy/mouse_proxy.hpp>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis;

namespace {
namespace m {
using namespace ruis::make;
} // namespace m
} // namespace

namespace {
constexpr auto color_dialog_surroundings = 0xb0000000;
} // namespace

namespace {
std::vector<utki::shared_ref<widget>> make_root_widget_structure(
	utki::shared_ref<ruis::context> c, //
	container::parameters container_params,
	utki::span<const utki::shared_ref<widget>> contents
)
{
	// clang-format off
    return {
        m::mouse_proxy(c,
            {
                .layout_params{
                    .dims{ruis::dim::fill, ruis::dim::fill}
                },
                .widget_params{
                    .id = "ruis_mouse_proxy"s
                }
            }
        ),
        m::rectangle(c,
            {
                .layout_params{
                    .dims{ruis::dim::fill, ruis::dim::fill}
                },
                .color_params{
                    .color = color_dialog_surroundings
                }
            }
        ),
        m::pile(c,
            {},
            {
                [&](){
                    auto mp = m::mouse_proxy(c,
                        {
                            .layout_params{
                                .dims{ruis::dim::fill, ruis::dim::fill}
                            }
                        }
                    );
                    mp.get().mouse_button_handler = [](mouse_proxy& w, const mouse_button_event&){return true;};
                    mp.get().mouse_move_handler = [](mouse_proxy& w, const mouse_move_event&){return true;};
                    mp.get().hovered_change_handler = [](mouse_proxy& w, unsigned pointer_id){return true;};
                    return mp;
                }(),
                m::nine_patch(c,
                    {
                        .widget_params{
                            .id = "ruis_nine_patch"s
                        },
                        .container_params = std::move(container_params),
                        .nine_patch_params{
                            .nine_patch = c.get().loader().load<ruis::res::nine_patch>("ruis_npt_window_bg")
                        }
                    },
                    contents
                )
            }
        )
    };
	// clang-format on
}
} // namespace

dialog::dialog(
	utki::shared_ref<ruis::context> context, //
	all_parameters params,
	utki::span<const utki::shared_ref<widget>> contents
) :
	widget(std::move(context), {}, {}),
	container(
		this->context, //
		{
			.container_params{
				.layout = ruis::layout::pile //
			} //
		},
		make_root_widget_structure(
			this->context, //
			std::move(params.container_params),
			contents
		)
	)
{
	{
		auto& mp = this->get_widget_as<ruis::mouse_proxy>("ruis_mouse_proxy"sv);
		mp.mouse_button_handler = [this](ruis::mouse_proxy&, const ruis::mouse_button_event& e) {
			this->on_dismiss();
			// consume event
			return true;
		};
	}
}

void dialog::on_dismiss()
{
	this->close();
}

void dialog::close()
{
	this->context.get().post_to_ui_thread([d = utki::make_shared_from(*this)]() {
		d.get().remove_from_parent();
	});
}
