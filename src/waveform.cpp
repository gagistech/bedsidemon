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
	this->value_offset = 0;
	this->value_max = 0xff;

	// this->px_per_ms = this->context.get().units.mm_to_px(25.0 / 1000.0); // 25 mm per second
	this->px_per_ms = 25.0 / 1000.0;

    constexpr auto default_gap_pp = 20;
    this->gap_px = this->context.get().units.pp_to_px(default_gap_pp);
}

void waveform::render(const ruis::matrix4& matrix)const {
	this->left_path_vao.render(matrix, 0xffffff00);
	this->right_path_vao.render(matrix, 0xffffff00);
}

void waveform::on_resize(){
	this->sweep_pos_px = 0;
	// TODO: clear all points
}

void waveform::push(ruis::real value, ruis::real dt_ms){
	auto dx = dt_ms * this->px_per_ms;

	// push new point
	if(this->left_points.empty()){
		this->left_points.push_back({0 , value});
	}else{
		this->left_points.push_back(
			{
				this->left_points.back().x() + dx,
				value
			}
		);
	}

	// wrap around left_points if needed
	ASSERT(!this->left_points.empty())
	if(this->left_points.back().x() >= this->rect().d.x()){
		auto dx1 = this->left_points.back().x() - this->rect().d.x();
		auto dx2 = dx - dx1;
		ASSERT(dx1 >= 0)
		ASSERT(dx2 >= 0)

		auto ratio = dx1 / dx;
		
		ASSERT(this->left_points.size() >= 2) // TODO: why?

		auto v = this->left_points.back().y();
		auto dv = v - std::next(this->left_points.rbegin())->y();

		auto dv1 = dv * ratio;
		auto dv2 = dv - dv1;

		this->left_points.back().x() = this->rect().d.x();
		this->left_points.back().y() -= dv2;

		// swap left<->right
		using std::swap;
		swap(this->left_points, this->right_points);
		this->left_points.clear();
		this->left_points.push_back({0, this->right_points.back().y()});
		this->left_points.push_back({dx2, v});
	}

	// pop point from right_points
	if(!this->right_points.empty()){
		// there should be at least one line segment
		ASSERT(this->right_points.size() >= 2)

		for(;;){
			auto tail_dx = std::next(this->right_points.begin())->x() - this->right_points.front().x();
			if(tail_dx <= dx){
				this->right_points.pop_front();
				if(this->right_points.size() == 1){
					this->right_points.clear();
					// TODO: pop from left_points
					break;
				}
				dx -= tail_dx;
				continue;
			}else{
				auto ratio = dx / tail_dx;
				auto tail_dv = std::next(this->right_points.begin())->y() - this->right_points.front().y();

				auto dv1 = tail_dv * ratio;

				this->right_points.front() += ruis::vector2{dx, dv1};

				break;
			}
		}
	}

	std::cout << "num_left = " << this->left_points.size() << ", num_right = " << this->right_points.size() << std::endl;

	this->make_vaos();
}

decltype(std::declval<ruis::path>().stroke()) waveform::make_vertices(const std::deque<ruis::vector2>& points){
	ASSERT(this->value_max > this->value_offset)
	auto scale = this->rect().d.y() / (this->value_max - this->value_offset);

	ruis::path path;
	for(const auto& p : points){
		auto v = this->rect().d.y() - p.y() * scale + this->value_offset;
		path.line_to(p.x(), v);
	}

	return path.stroke();
}

void waveform::make_vaos(){
	this->left_path_vao.set(this->make_vertices(this->left_points));
	this->right_path_vao.set(this->make_vertices(this->right_points));
}
