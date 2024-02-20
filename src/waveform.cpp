#include "waveform.hpp"

using namespace bedsidemon;

waveform::waveform(
	utki::shared_ref<ruis::context> context,
	ruis::widget::parameters widget_params,
	ruis::color_widget::parameters color_params
) :
	ruis::widget(std::move(context), std::move(widget_params)),
	ruis::color_widget(this->context, std::move(color_params)),
	paths{{
		{.vao{this->context.get().renderer}},
		{.vao{this->context.get().renderer}}
	}}
{
	this->value_offset = 0;
	this->value_max = 0xff;

	// this->px_per_ms = this->context.get().units.mm_to_px(25.0 / 1000.0); // 25 mm per second
	this->px_per_ms = 25.0 / 1000.0;

    constexpr auto default_gap_pp = 20;
    this->gap_px = this->context.get().units.pp_to_px(default_gap_pp);
}

void waveform::render(const ruis::matrix4& matrix)const {
	for(const auto& pv : this->paths){
		pv.vao.render(matrix, 0xffffff00);
	}
}

void waveform::on_resize(){
	for(auto& p : this->paths){
		p.points.clear();
	}
	this->make_vaos();
}

void waveform::push(ruis::real value, ruis::real dt_ms){
	auto dx = dt_ms * this->px_per_ms;

	// push new point
	if(this->paths[0].points.empty()){
		this->paths[0].points.push_back({0 , value});
	}else{
		this->paths[0].points.push_back(
			{
				this->paths[0].points.back().x() + dx,
				value
			}
		);
	}

	// wrap around paths[0].points if needed
	ASSERT(!this->paths[0].points.empty())
	if(this->paths[0].points.back().x() >= this->rect().d.x()){
		auto dx1 = this->paths[0].points.back().x() - this->rect().d.x();
		auto dx2 = dx - dx1;
		ASSERT(dx1 >= 0)
		ASSERT(dx2 >= 0)

		auto ratio = dx1 / dx;
		
		ASSERT(this->paths[0].points.size() >= 2) // TODO: why?

		auto v = this->paths[0].points.back().y();
		auto dv = v - std::next(this->paths[0].points.rbegin())->y();

		auto dv1 = dv * ratio;
		auto dv2 = dv - dv1;

		this->paths[0].points.back().x() = this->rect().d.x();
		this->paths[0].points.back().y() -= dv2;

		// swap left<->right
		using std::swap;
		swap(this->paths[0].points, this->paths[1].points);
		this->paths[0].points.clear();
		this->paths[0].points.push_back({0, this->paths[1].points.back().y()});
		this->paths[0].points.push_back({dx2, v});
	}

	// pop point from points[1]
	if(!this->paths[1].points.empty()){
		// there should be at least one line segment
		ASSERT(this->paths[1].points.size() >= 2)

		for(;;){
			auto tail_dx = std::next(this->paths[1].points.begin())->x() - this->paths[1].points.front().x();
			if(tail_dx <= dx){
				this->paths[1].points.pop_front();
				if(this->paths[1].points.size() == 1){
					this->paths[1].points.clear();
					// TODO: pop from paths[0].points
					break;
				}
				dx -= tail_dx;
				continue;
			}else{
				auto ratio = dx / tail_dx;
				auto tail_dv = std::next(this->paths[1].points.begin())->y() - this->paths[1].points.front().y();

				auto dv1 = tail_dv * ratio;

				this->paths[1].points.front() += ruis::vector2{dx, dv1};

				break;
			}
		}
	}

	std::cout << "num_left = " << this->paths[0].points.size() << ", num_right = " << this->paths[1].points.size() << std::endl;

	this->make_vaos();
}

void waveform::make_vaos(){
	ASSERT(this->value_max > this->value_offset)
	auto scale = this->rect().d.y() / (this->value_max - this->value_offset);

	for(auto& pv : this->paths){
		ruis::path path;
		for(const auto& p : pv.points){
			auto v = this->rect().d.y() - p.y() * scale + this->value_offset;
			path.line_to(p.x(), v);
		}

		pv.vao.set(path.stroke());
	}
}
