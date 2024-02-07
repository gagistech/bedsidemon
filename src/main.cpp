/*
bedsidemon - Bed-side monitor example GUI project

Copyright (C) Ivan Gagis  <igagis@gmail.com>

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

#include <ruisapp/application.hpp>

#include "spo2/contec_cms50d_plus.hpp"
#include "spo2/spo2_parameter_window.hpp"

namespace bedsidemon {

class application : public ruisapp::application
{
	std::unique_ptr<contec_cms50d_plus> spo2_sensor;

public:
	application() :
		ruisapp::application( //
			"ruis-tests",
			[]() {
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));
				return wp;
			}()
		)
	{
		this->gui.init_standard_widgets(*this->get_res_file());

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file("res/"));

		auto c = this->gui.context.get().inflater.inflate(*this->get_res_file("res/main.gui"));
		this->gui.set_root(c);

		auto& pw_container = c.get().get_widget_as<ruis::container>("pw_container");

		auto pw = utki::make_shared<spo2_parameter_window>(this->gui.context);
		this->spo2_sensor = std::make_unique<contec_cms50d_plus>(pw, "/dev/ttyUSB0");

		pw_container.push_back(pw);
	}
};

const ruisapp::application_factory app_fac([](auto args) {
	return std::make_unique<application>();
});

} // namespace bedsidemon
