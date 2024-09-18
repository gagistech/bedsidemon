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

#include "spo2_parameter_window.hpp"

#include <ratio>
#include <vector>

#include <ruis/layout/layout.hpp>
#include <ruis/widget/label/image.hpp>
#include <ruis/widget/label/margins.hpp>
#include <ruis/widget/label/rectangle.hpp>
#include <ruis/widget/proxy/click_proxy.hpp>

#include "../application.hpp"
#include "../style.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace bedsidemon::m {
using namespace bedsidemon::make;
}; // namespace bedsidemon::m

namespace {
constexpr auto heart_blink_time_ms = 150;
} // namespace

namespace {

constexpr auto color_main_value = 0xffffff00;
constexpr auto color_secondary_value = 0xff00ffff;

constexpr auto font_size_label = 16_pp;
constexpr auto font_size_main_value = 60_pp;
constexpr auto font_size_secondary_value = 40_pp;

constexpr auto heart_size = 15_pp;

std::vector<utki::shared_ref<ruis::widget>> make_numeric_content(
	utki::shared_ref<ruis::context> c,
	std::u32string title
)
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
            std::move(title)
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

std::vector<utki::shared_ref<ruis::widget>> make_widgets(utki::shared_ref<ruis::context> c, std::u32string title)
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
                        m::pile(c,
                            {
                                .layout_params{
                                    .dims = {ruis::dim::fill, ruis::dim::min}
                                }
                            },
                            {
                                m::click_proxy(c,
                                    {
                                        .layout_params{
                                            .dims{ruis::dim::fill, ruis::dim::fill}
                                        },
                                        .widget_params{
                                            .id = "click_proxy"s
                                        }
                                    }
                                ),
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
                                    make_numeric_content(c, title)
                                )
                            }
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

spo2_parameter_window::spo2_parameter_window(utki::shared_ref<ruis::context> context, std::u32string title) :
	ruis::widget( //
		std::move(context),
		{//
            .dims = {ruis::dim::fill, ruis::dim::min}
        },
        {}
	),
    ruis::color_widget(
        this->context,
        ruis::color_widget::parameters{
            .color = color_main_value
        }
    ),
	ruis::container( //
		this->context,
		{.container_params =
			 {//
			  .layout = ruis::layout::row
			 }},
		make_widgets(this->context, title)
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
	{
		auto& cp = this->get_widget_as<ruis::click_proxy>("click_proxy"sv);
		cp.click_handler = [this](ruis::click_proxy& cp) {
			if (this->pw_menu) {
				// menu is already open
				return;
			}
			auto pwm = utki::make_shared<spo2_parameter_window_menu>(this->context, utki::make_weak_from(*this));
			this->pw_menu = pwm;
			application::inst().open_menu(pwm);
		};
	}

	auto& ss = settings_storage::inst();
	decltype(settings_storage::settings_changed_signal
	)::callback_type settings_change_handler = [&pw = *this](const settings& s) {
		pw.waveform.set_sweep_speed(ruis::real(s.sweep_speed_um_per_sec) / ruis::real(std::milli::den));
	};
	settings_change_handler(ss.get());
	this->settings_change_signal_connection = ss.settings_changed_signal.connect(std::move(settings_change_handler));
}

spo2_parameter_window::~spo2_parameter_window()
{
	// in case of application exit, the parameter window widgets will be destroyed by
	// ruisapp::application::gui destructor, which is after the settings_storage,
	// so we need to check if the settings_storage object still exists before
	// disconnecting from its signal
	if (settings_storage::is_created()) {
		auto& ss = settings_storage::inst();
		ss.settings_changed_signal.disconnect(this->settings_change_signal_connection);
	}
}

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

void spo2_parameter_window::on_color_change()
{
	this->spo2_value.set_color(this->get_color());
	this->waveform.set_color(this->get_color());
}
