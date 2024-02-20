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

	std::deque<ruis::vector2> left_points;
	std::deque<ruis::vector2> right_points;

	ruis::real value_offset;
	ruis::real value_max;

	ruis::real px_per_ms;

	// TODO: is needed?
	ruis::real sweep_pos_px = 0;

    ruis::real gap_px;

public:
	waveform(
		utki::shared_ref<ruis::context> context,
		ruis::widget::parameters widget_params,
		ruis::color_widget::parameters color_params
	);

	void render(const ruis::matrix4& matrix) const override;

	void on_resize()override;

	void push(ruis::real value, ruis::real dt_ms);

private:
	decltype(std::declval<ruis::path>().stroke()) make_vertices(const std::deque<ruis::vector2>& points);

	void make_vaos();
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
