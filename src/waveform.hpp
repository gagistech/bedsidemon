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

#pragma once

#include <deque>

#include <ruis/paint/path_vao.hpp>
#include <ruis/widgets/base/color_widget.hpp>
#include <ruis/widgets/widget.hpp>

namespace bedsidemon {

class waveform :
	virtual public ruis::widget, //
	public ruis::color_widget
{
	struct path {
		ruis::path_vao vao;
		ruis::vector2 origin;

		std::deque<ruis::vector2> points;
	};

	std::array<path, 2> paths;

	ruis::real value_offset{0};
	ruis::real value_max;

	ruis::real px_per_ms;

	ruis::real gap_px;

public:
	struct all_parameters {
		ruis::widget::parameters widget_params;
		ruis::color_widget::parameters color_params;
	};

	waveform(utki::shared_ref<ruis::context> context, all_parameters params);

	void render(const ruis::matrix4& matrix) const override;

	void on_resize() override;

	void push(ruis::real value, ruis::real dt_ms);

private:
	void make_vaos();
};

namespace make {

inline utki::shared_ref<ruis::widget> waveform( //
	utki::shared_ref<ruis::context> context,
	waveform::all_parameters params
)
{
	return utki::make_shared<bedsidemon::waveform>(std::move(context), std::move(params));
}
} // namespace make

} // namespace bedsidemon
