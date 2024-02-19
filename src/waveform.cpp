#include "waveform.hpp"

using namespace bedsidemon;

waveform::waveform(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::color_widget::parameters color_params
) :
	ruis::widget(std::move(context), std::move(widget_params)),
	ruis::color_widget(this->context, std::move(color_params))
{}
