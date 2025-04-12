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

#include <ruis/widget/button/base/button.hpp>
#include <ruis/widget/label/margins.hpp> // TODO: should be moved into rectangle
#include <ruis/widget/label/rectangle.hpp>

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false positive")
class rectangle_button :
	virtual public button, //
	// no reason to make rectangle inheritance private
	public rectangle,
	private margins // TODO: should be moved into rectangle
{
public:
	struct parameters {
		constexpr static const auto default_pressed_color = 0xffaaaaaa;
		uint32_t pressed_color = default_pressed_color;

		constexpr static const auto default_unpressed_color = 0xff666666;
		uint32_t unpressed_color = default_unpressed_color;

		// TODO: add disabled_color ?
	};

	void render(const matrix4& matrix) const override;

private:
	parameters params;

	void update_color();

protected:
	void on_pressed_change() override;

	rectangle_button( //
		utki::shared_ref<ruis::context> context,
        ruis::container::parameters container_params,
		ruis::frame_widget::parameters frame_params,
		parameters params,
        ruis::widget_list contents
	);
};

} // namespace ruis
