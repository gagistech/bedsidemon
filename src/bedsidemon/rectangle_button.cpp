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

#include "rectangle_button.hpp"

using namespace ruis;

void rectangle_button::render(const matrix4& matrix) const
{
	this->rectangle::render(matrix);
	this->margins::render(matrix);
}

void rectangle_button::on_pressed_change()
{
	this->update_color();
	this->button::on_pressed_change();
}

void rectangle_button::update_color()
{
	if (this->is_pressed()) {
		this->set_color(this->params.pressed_color);
	} else {
		this->set_color(this->params.unpressed_color);
	}
}

rectangle_button::rectangle_button( //
		utki::shared_ref<ruis::context> context,
        ruis::container::parameters container_params,
		ruis::frame_widget::parameters frame_params,
		parameters params,
        utki::span<const utki::shared_ref<ruis::widget>> contents
	):
    widget(std::move(context), {}, {}),
	button(this->context, button::parameters{}),
	rectangle(
		this->context,
		rectangle::all_parameters{}
	),
    margins(
        this->context,
        {
            .container_params = std::move(container_params), //
            .frame_params = std::move(frame_params)
        },
        contents
    ),
	params(std::move(params))
{
	this->update_color();
}
