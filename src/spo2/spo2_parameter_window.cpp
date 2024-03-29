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

#include "spo2_parameter_window.hpp"

#include <ratio>
#include <vector>

#include <ruis/layouts/layout.hpp>
#include <ruis/widgets/label/rectangle.hpp>

using namespace std::string_literals;

using namespace bedsidemon;

namespace {
namespace m {
using namespace ruis::make;
using namespace bedsidemon::make;
}; // namespace m

std::vector<utki::shared_ref<ruis::widget>> build_layout(utki::shared_ref<ruis::context> c)
{
	using ruis::lp;

	constexpr auto color_border = 0xff808080;
	constexpr auto color_info_text = 0xff808080;
	constexpr auto color_main_value = 0xffffff00;
	constexpr auto color_secondary_value = 0xff00ffff;

	constexpr auto font_size_label = 16;

	constexpr auto font_size_main_value_pp = 60;
	auto font_size_main_value = c.get().units.pp_to_px(font_size_main_value_pp);

	constexpr auto font_size_secondary_value_pp = 40;
	auto font_secondary_main_value = c.get().units.pp_to_px(font_size_secondary_value_pp);

	// clang-format off
    return {
        m::waveform(c,
            {
                .widget_params = {
                    .id = "pw_waveform"s,
                    .lp = {
                        .dims = {lp::fill, lp::fill},
                        .weight = 3
                    }
                },
                .color_params = {
                    .color = color_main_value
                }
            }
        ),
        m::rectangle(c,
            {
                .widget_params = {
                    .lp = {
                        .dims = {c.get().units.pp_to_px(1), lp::fill}
                    }
                },
                .color_params = {
                    .color = color_border
                }
            }
        ),
        m::container(c,
            {
                .widget_params = {
                    .lp = {
                        .dims = {lp::fill, lp::min},
                        .weight = 1
                    }
                },
                .container_params = {
                    .layout = ruis::layout::column
                }
            },
            {
                m::text(c,
                    {
                        .widget_params = {
                            .lp = {
                                .align = {lp::align::front, lp::align::front}
                            }
                        },
                        .color_params = {
                            .color = color_info_text
                        },
                        .text_params = {
                            .font_size = font_size_label
                        }
                    },
                    U"SpO2 %"s
                ),
                m::text(c,
                    {
                        .widget_params = {
                            .id = "spo2_value"s
                        },
                        .color_params = {
                            .color = color_main_value
                        },
                        .text_params = {
                            .font_size = font_size_main_value
                        }
                    },
                    U"---"s
                ),
                m::text(c,
                    {
                        .widget_params = {
                            .id = "bpm_value"s
                        },
                        .color_params = {
                            .color = color_secondary_value
                        },
                        .text_params = {
                            .font_size = font_secondary_main_value
                        }
                    },
                    U"---"s
                ),
                
                m::rectangle(c,
                    {
                        .widget_params = {
                            .lp = {
                                .dims = {lp::fill, c.get().units.pp_to_px(1)}
                            }
                        },
                        .color_params = {
                            .color = color_border
                        }
                    }
                )
            }
        )
    };
	// clang-format on
}
} // namespace

spo2_parameter_window::spo2_parameter_window(utki::shared_ref<ruis::context> context) :
	ruis::widget( //
		std::move(context),
		{//
		 .widget_params =
			 {.lp =
				  {//
				   .dims = {ruis::lp::fill, ruis::lp::min}
				  }}
		}
	),
	ruis::container( //
		this->context,
		{.container_params =
			 {//
			  .layout = ruis::layout::row
			 }},
		build_layout(this->context)
	),
	spo2_value(this->get_widget_as<ruis::text>("spo2_value")),
	bpm_value(this->get_widget_as<ruis::text>("bpm_value")),
	waveform(this->get_widget_as<bedsidemon::waveform>("pw_waveform"))
{}

void spo2_parameter_window::set(const spo2_measurement& meas)
{
	// set oxygenation
	if (meas.spo2 == 0 || meas.spo2 > std::centi::den) {
		// invalid value
		this->spo2_value.set_text("---");
	} else {
		this->spo2_value.set_text(std::to_string(unsigned(meas.spo2)));
	}

	constexpr auto bpm_invalid_value = 0xff;

	// set bpm
	if (meas.pulse_rate == 0 || meas.pulse_rate == bpm_invalid_value) {
		// invalid value
		this->bpm_value.set_text("---");
	} else {
		this->bpm_value.set_text(std::to_string(unsigned(meas.pulse_rate)));
	}

	this->waveform.push(meas.waveform_point, meas.delta_time_ms);
}
