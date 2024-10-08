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

#pragma once

#include <ruis/widget/button/push_button.hpp>

#include "rectangle_button.hpp"

namespace ruis {

class rectangle_push_button :
	virtual public ruis::push_button, //
	public rectangle_button
{
public:
	struct all_parameters {
		layout_parameters layout_params;
		widget::parameters widget_params;
		ruis::container::parameters container_params;
		ruis::frame_widget::parameters frame_params;
		button::parameters button_params;
		rectangle_button::parameters rectangle_button_params;
	};

	rectangle_push_button(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		utki::span<const utki::shared_ref<ruis::widget>> contents
	);

	void on_pressed_change() override;

	bool on_mouse_button(const mouse_button_event& e) override
	{
		return this->push_button::on_mouse_button(e);
	}

	void on_hovered_change(unsigned pointer_id) override
	{
		this->push_button::on_hovered_change(pointer_id);
	}
};

namespace make {
inline utki::shared_ref<ruis::rectangle_push_button> rectangle_push_button(
	utki::shared_ref<ruis::context> context,
	ruis::rectangle_push_button::all_parameters params,
	utki::span<const utki::shared_ref<ruis::widget>> contents
)
{
	return utki::make_shared<ruis::rectangle_push_button>(
		std::move(context), //
		std::move(params),
		contents
	);
}
} // namespace make

} // namespace ruis
