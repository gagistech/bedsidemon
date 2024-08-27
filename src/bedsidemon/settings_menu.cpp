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

using namespace bedsidemon;

settings_menu::settings_menu(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context), //
		{
			.dims = {ruis::lp::fill, ruis::lp::fill}
},
		{}
	),
	ruis::nine_patch(
		this->context, //
		{
			.nine_patch_params =
				{
					.nine_patch = this->context.get().loader.load<ruis::res::nine_patch>("ruis_npt_window_bg") //
				} //
		},
		{}
	)
{}
