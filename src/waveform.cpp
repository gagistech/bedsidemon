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
	this->value_max = 100;

	// this->px_per_ms = this->context.get().units.mm_to_px(25.0 / 1000.0); // 25 mm per second
	this->px_per_ms = 25.0 / 1000.0;

    constexpr auto default_gap_pp = 20;
    this->gap_px = this->context.get().units.pp_to_px(default_gap_pp);
}

void waveform::render(const ruis::matrix4& matrix)const {
	for(const auto& pv : this->paths){
		pv.vao.render(ruis::matrix4(matrix).translate(pv.origin), this->get_color());
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
	// dx can be 0 when it is a very first sample received from sensor, dt_ms == 0 in this case and => dx == 0
	ASSERT(dx >= 0, [&](auto&o){o << "dx = " << dx << ", dt_ms = " << dt_ms << ", this->px_per_ms = " << this->px_per_ms;})

	// push new point
	if(this->paths[0].points.empty()){
		this->paths[0].points.push_back({0 , value});
	}else{
		if(dx == 0){
			// if first sample received from sensor we don't know the delta time from previous sample,
			// so just update the latest value
			this->paths[0].points.back().y() = value;
		}else{
			this->paths[0].points.push_back(
				{
					this->paths[0].points.back().x() + dx,
					value
				}
			);
		}
	}

	ASSERT(!this->paths[0].points.empty())

	// wrap around paths[0].points if needed
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

	auto sweep_pos = this->paths[0].points.back().x();
	auto pop_pos = sweep_pos + this->gap_px;

	// pop points from another path
	auto& pop_path = [&]() -> path& {
		if(pop_pos >= this->rect().d.x()){
			pop_pos -= this->rect().d.x();
			return this->paths[0];
		}
		return this->paths[1];
	}();

	if(!pop_path.points.empty()){
		// there should be at least one line segment
		ASSERT(pop_path.points.size() >= 2)

		for(;;){
			if(std::next(pop_path.points.begin())->x() < pop_pos){
				pop_path.points.pop_front();
				if(pop_path.points.size() == 1){
					pop_path.points.clear();
					break;
				}
				continue;
			}else{
				auto tail_x = pop_path.points.front().x();
				auto tail_dx = std::next(pop_path.points.begin())->x() - tail_x;
				ASSERT(tail_dx > 0)
				if(tail_dx <= 0){
					pop_path.points.pop_front();
					if(pop_path.points.size() == 1){
						pop_path.points.clear();
						break;
					}
					continue;
				}
				auto ratio = (pop_pos - tail_x) / tail_dx;
				auto tail_dv = std::next(pop_path.points.begin())->y() - pop_path.points.front().y();

				auto dv1 = tail_dv * ratio;

				pop_path.points.front() += ruis::vector2{dx, dv1};

				break;
			}
		}
	}

	std::cout << "num_left = " << this->paths[0].points.size() << ", num_right = " << this->paths[1].points.size() << std::endl;

	this->make_vaos();
}

template <typename T>
struct skip
{
    T& t;
    size_t n;
    skip(T& v, size_t s) : t(v), n(s) {}
	
    auto begin() -> typename T::iterator
    {
		ASSERT(!t.empty())
		using std::begin;
        return utki::next(begin(t), n);
    }
    auto end() -> typename T::iterator
    {
		using std::end;
        return end(t);
    }
};

void waveform::make_vaos(){
	ASSERT(this->value_max > this->value_offset)
	const auto& height = this->rect().d.y();
	auto scale = height / (this->value_max - this->value_offset);

	for(auto& pv : this->paths){
		if(pv.points.empty()){
			continue;
		}

		pv.origin = {
			pv.points.front().x(),
			height - pv.points.front().y() * scale + this->value_offset
		};

		ruis::path path;
		for(const auto& p : skip(pv.points, 1)){
			ruis::vector2 point = {
				p.x(),
				height - p.y() * scale + this->value_offset
			};
			path.line_to(point - pv.origin);
		}

		pv.vao.set(path.stroke(0.5));
	}
}
