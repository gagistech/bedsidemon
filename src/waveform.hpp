#pragma once

#include <deque>

#include <ruis/paint/path_vao.hpp>
#include <ruis/widget.hpp>
#include <ruis/widgets/base/color_widget.hpp>

namespace bedsidemon {

class waveform :
	virtual public ruis::widget, //
	public ruis::color_widget
{
	ruis::path_vao left_path_vao;
	ruis::path_vao right_path_vao;

	struct point {
		ruis::real delta_time_ms;
		ruis::real value;
	};

	std::deque<point> left_points;
	std::deque<point> right_points;

	ruis::real value_min;
	ruis::real value_max;

	ruis::real px_per_ms;

	ruis::real sweep_pos = 0;

    ruis::real gap_px;

public:
	waveform(
		utki::shared_ref<ruis::context> context,
		ruis::widget::parameters widget_params,
		ruis::color_widget::parameters color_params
	);

	void render(const ruis::matrix4& matrix) const override;
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
