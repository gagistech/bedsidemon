#include "waveform.hpp"

using namespace bedsidemon;

waveform::waveform(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::color_widget::parameters color_params
) :
	ruis::widget(std::move(context), std::move(widget_params)),
	ruis::color_widget(this->context, std::move(color_params)),
	left_path_vao(this->context.get().renderer),
	right_path_vao(this->context.get().renderer)
{
	this->value_min = 0;
	this->value_max = 0xff;

	// this->px_per_ms = this->context.get().units.mm_to_px(25.0 / 1000.0); // 25 mm per second
	this->px_per_ms = 25.0 / 1000.0;

    constexpr auto default_gap_pp = 20;
    this->gap_px = this->context.get().units.pp_to_px(default_gap_pp);
}

void waveform::render(const ruis::matrix4& matrix)const {

}
