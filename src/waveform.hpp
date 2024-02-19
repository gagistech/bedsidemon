#pragma once

#include <ruis/widget.hpp>
#include <ruis/widgets/base/color_widget.hpp>

namespace bedsidemon {

class waveform :
	virtual public ruis::widget, //
	public ruis::color_widget
{
public:
	waveform(
		utki::shared_ref<ruis::context> context,
		ruis::widget::parameters widget_params,
		ruis::color_widget::parameters color_params
	);
};

namespace make {

struct waveform_parameters {
	ruis::widget::parameters widget_params;
	ruis::color_widget::parameters color_params;
};

inline utki::shared_ref<ruis::widget> waveform( //
	utki::shared_ref<ruis::context> context,
	waveform_parameters params
)
{
	return utki::make_shared<bedsidemon::waveform>(
		std::move(context),
		std::move(params.widget_params),
		std::move(params.color_params)
	);
}
} // namespace make

} // namespace bedsidemon
