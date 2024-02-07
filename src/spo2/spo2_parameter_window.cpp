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

#include "spo2_parameter_window.hpp"

using namespace bedsidemon;

namespace {
const tml::forest pw_layout = tml::read(R"qwertyuiop(
    lp{
        dx{fill}
    }

    layout{row}

    // TODO: this doesn't work
    defs{
        color_border{0xff808080}
        color_main_value{0xff00ffff}
    }

    @widget{
        lp{
            dx{0}
            weight{3}
        }
        id{pw_waveform}
    }

    @color{
        lp{
            dx{1pp}
            dy{fill}
        }
        color{${color_border}}
    }

    @column{
        lp{
            dx{min}
            dy{min}
            weight{1}
        }

        @text{
            id{spo2_value}
            text{---}
            font_size{40pp}
            color{${color_main_value}}
        }

        @color{
            lp{
                dx{fill}
                dy{1pp}
            }
            color{${color_border}}
        }
    }
)qwertyuiop");
} // namespace

spo2_parameter_window::spo2_parameter_window(utki::shared_ref<ruis::context> context) :
	ruis::widget(std::move(context), pw_layout),
	ruis::container(this->context, pw_layout),
	spo2_value(this->get_widget_as<ruis::text>("spo2_value"))
{}

void spo2_parameter_window::set(const spo2_measurement& meas)
{
	// set oxygenation
    if(meas.spo2 == 0){
        // invalid value
        this->spo2_value.set_text("---");
    }else{
        this->spo2_value.set_text(std::to_string(unsigned(meas.spo2)));
    }
}