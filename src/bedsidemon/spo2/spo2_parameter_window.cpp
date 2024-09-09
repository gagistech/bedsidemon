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

#include <ruis/layout/layout.hpp>
#include <ruis/widget/group/margins.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/label/rectangle.hpp>

#include "../application.hpp"
#include "../style.hpp"

using namespace std::string_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace {
constexpr auto heart_blink_time_ms = 150;
} // namespace

namespace {
namespace m {
using namespace ruis::make;
using namespace bedsidemon::make;
}; // namespace m

using namespace ruis::length_literals;

constexpr auto color_main_value = 0xffffff00;
constexpr auto color_secondary_value = 0xff00ffff;

constexpr auto font_size_label = 16_pp;
constexpr auto font_size_main_value = 60_pp;
constexpr auto font_size_secondary_value = 40_pp;

constexpr auto heart_size = 15_pp;

std::vector<utki::shared_ref<ruis::widget>> make_numeric_content(utki::shared_ref<ruis::context> c)
{
	// clang-format off
    return {
        m::text(c,
            {
                .layout_params = {
                    .align = {ruis::align::front, ruis::align::front}
                },
                .color_params = {
                    .color = style::color_info_text
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
        m::row(c,
            {},
            {
                m::text(c,
                    {
                        .widget_params = {
                            .id = "bpm_value"s
                        },
                        .color_params = {
                            .color = color_secondary_value
                        },
                        .text_params = {
                            .font_size = font_size_secondary_value
                        }
                    },
                    U"---"s
                ),
                m::image(c,
                    {
                        .layout_params = {
                            .dims = {heart_size, ruis::dim::min},
                            .align = {ruis::align::front, ruis::align::front}
                        },
                        .widget_params = {
                            .id = "heart"s,
                            .visible = false
                        },
                        .image_params = {
                            .img = c.get().loader.load<ruis::res::image>("img_heart"),
                            .keep_aspect_ratio = true
                        }
                    }
                )
            }
        )
    };
	// clang-format on
}

std::vector<utki::shared_ref<ruis::widget>> make_widgets(utki::shared_ref<ruis::context> c)
{
	// clang-format off
    return {
        m::waveform(c,
            {
                .layout_params = {
                    .dims = {ruis::dim::fill, ruis::dim::fill},
                    .weight = 3
                },
                .widget_params = {
                    .id = "pw_waveform"s,
                    .clip = true
                },
                .color_params = {
                    .color = color_main_value
                }
            }
        ),
        m::row(c,
            {
                .layout_params = {
                    .dims = {ruis::dim::fill, ruis::dim::min},
                    .weight = 1
                }
            },
            {
                m::rectangle(c,
                    {
                        .layout_params = {
                            .dims = {1_pp, ruis::dim::fill}
                        },
                        .color_params = {
                            .color = style::color_border
                        }
                    }
                ),
                m::column(c,
                    {
                        .layout_params = {
                            .dims = {ruis::dim::fill, ruis::dim::min},
                            .weight = 1
                        }
                    },
                    {
                        m::margins(c,
                            {
                                .layout_params = {
                                    .dims = {ruis::dim::fill, ruis::dim::min}
                                },
                                .container_params = {
                                    .layout = ruis::layout::column
                                },
                                .frame_params = {
                                    .borders = {style::pw_padding}
                                }
                            },
                            make_numeric_content(c)
                        ),
                        m::rectangle(c,
                            {
                                .layout_params = {
                                    .dims = {ruis::dim::fill, 1_pp}
                                },
                                .color_params = {
                                    .color = style::color_border
                                }
                            }
                        )
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
            .dims = {ruis::dim::fill, ruis::dim::min}
        },
        {}
	),
	ruis::container( //
		this->context,
		{.container_params =
			 {//
			  .layout = ruis::layout::row
			 }},
		make_widgets(this->context)
	),
	spo2_value(this->get_widget_as<ruis::text>("spo2_value")),
	bpm_value(this->get_widget_as<ruis::text>("bpm_value")),
	heart(this->get_widget("heart")),
	waveform(this->get_widget_as<bedsidemon::waveform>("pw_waveform")),
	heart_timer(utki::make_shared<ruis::timer>(//
        this->context.get().updater,
        [this](uint32_t elapsed_ms) {
		    this->on_heart_timer_expired();
	    }
    ))
{
	auto& ss = bedsidemon::application::inst().settings_storage;
	decltype(settings_storage::settings_changed_signal
	)::callback_type settings_change_handler = [&pw = *this](const settings& s) {
		pw.waveform.set_sweep_speed(ruis::real(s.sweep_speed_um_per_sec) / ruis::real(std::milli::den));
	};
	settings_change_handler(ss.get());
	this->settings_change_signal_connection = ss.settings_changed_signal.connect(std::move(settings_change_handler));
}

spo2_parameter_window::~spo2_parameter_window() = default;

// {
// TODO: disconnect settings change observer
// auto& ss = bedsidemon::application::inst().settings_storage;
// ss.settings_changed_signal.disconnect(this->settings_change_signal_connection);
// }

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

	if (meas.pulse_beat) {
		this->trigger_heart();
	}

	this->waveform.push(meas.waveform_point, meas.delta_time_ms);
}

void spo2_parameter_window::trigger_heart()
{
	this->heart.set_visible(true);
	auto& timer = this->heart_timer.get();
	timer.stop();
	timer.start(heart_blink_time_ms);
}

void spo2_parameter_window::on_heart_timer_expired()
{
	this->heart.set_visible(false);
	// std::cout << "heart" << std::endl;
}
