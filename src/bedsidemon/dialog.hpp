/*
bedsidemon - Bedside monitor example GUI project

Copyright (C) 2024-2025 Gagistech Oy <gagistechoy@gmail.com>

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

#include <ruis/widget/container.hpp>

namespace ruis {

// NOLINTNEXTLINE(bugprone-incorrect-enable-shared-from-this, "false positive")
class dialog :
	virtual public widget, //
	private container
{
public:
	struct all_parameters {
		container::parameters container_params;
	};

	dialog(
		utki::shared_ref<ruis::context> context, //
		all_parameters params,
		ruis::widget_list contents
	);

	void close();

	/**
	 * @brief On dismiss callback.
	 * This virtual function is called when the dialog is dismissed. For example when user clicks outside of the dialog.
	 * Default implementation just closes the dialog by calling its close() function.
	 */
	virtual void on_dismiss();
};

} // namespace ruis
