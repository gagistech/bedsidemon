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

#include "waveform.hpp"

#include <ratio>

using namespace bedsidemon;

namespace {
constexpr auto default_max_value = 100;
} // namespace

waveform::waveform(utki::shared_ref<ruis::context> context, all_parameters params) :
	ruis::widget(
		std::move(context),
		{
			.widget_params = std::move(params.widget_params)
}
	),
	ruis::color_widget(this->context, std::move(params.color_params)),
	paths{{//
		   {.vao{this->context.get().renderer}},
		   {.vao{this->context.get().renderer}}
	}},
	value_max(default_max_value)
{
	constexpr auto default_sweep_speed_mm_per_sec = 100;
	// this->px_per_ms =
	// this->context.get().units.mm_to_px(default_sweep_speed_mm_per_sec /
	// 1000.0);
	this->px_per_ms = ruis::real(default_sweep_speed_mm_per_sec) / std::milli::den;

	constexpr auto default_gap_pp = 30;
	this->gap_px = this->context.get().units.pp_to_px(default_gap_pp);
}

void waveform::render(const ruis::matrix4& matrix) const
{
	for (const auto& pv : this->paths) {
		pv.vao.render(ruis::matrix4(matrix).translate(pv.origin), this->get_color());
	}
}

void waveform::clear()
{
	for (auto& p : this->paths) {
		p.points.clear();
	}
	this->clear_accumulated_value();
	this->make_vaos();
}

void waveform::on_resize()
{
	this->clear();
}

void waveform::clear_accumulated_value()
{
	this->accumulated_value = 0;
	this->accumulated_dx = 0;
	this->num_values_accumulated = 0;
}

void waveform::push(ruis::real value, ruis::real dt_ms)
{
	auto dx = dt_ms * this->px_per_ms;

	this->accumulated_dx += dx;
	this->accumulated_value += value;
	++this->num_values_accumulated;

	if (this->accumulated_dx < this->min_step_px) {
		return;
	}

	value = this->accumulated_value / ruis::real(this->num_values_accumulated);
	dx = this->accumulated_dx;

	this->clear_accumulated_value();

	// dx can be 0 when it is a very first sample received from sensor, dt_ms == 0
	// in this case and => dx == 0
	ASSERT(dx > 0 || (dx == 0 && dt_ms == 0), [&](auto& o) {
		o << "dx = " << dx << ", dt_ms = " << dt_ms << ", this->px_per_ms = " << this->px_per_ms;
	})

	// push new point
	if (this->paths[0].points.empty()) {
		this->paths[0].points.push_back({0, value});
		return;
	} else {
		if (dx == 0) {
			// if first sample received from sensor we don't know the delta time from
			// previous sample, so just update the latest value
			this->paths[0].points.back().y() = value;
			return;
		} else {
			this->paths[0].points.push_back({this->paths[0].points.back().x() + dx, value});
		}
	}

	ASSERT(!this->paths[0].points.empty())

	// wrap around paths[0].points if needed
	if (this->paths[0].points.back().x() >= this->rect().d.x()) {
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
		if (pop_pos >= this->rect().d.x()) {
			pop_pos -= this->rect().d.x();
			return this->paths[0];
		}
		return this->paths[1];
	}();

	if (!pop_path.points.empty()) {
		// there should be at least one line segment
		ASSERT(pop_path.points.size() >= 2)

		for (; pop_path.points.size() >= 2;) {
			if (std::next(pop_path.points.begin())->x() <= pop_pos) {
				pop_path.points.pop_front();
				continue;
			} else {
				auto tail_x = pop_path.points.front().x();
				auto tail_dx = std::next(pop_path.points.begin())->x() - tail_x;
				ASSERT(tail_dx > 0, [&](auto& o) {
					o << "tail_dx = " << tail_dx << ", pop_path.size() = " << pop_path.points.size();
				})

				auto ratio = (pop_pos - tail_x) / tail_dx;
				auto tail_dv = std::next(pop_path.points.begin())->y() - pop_path.points.front().y();

				auto dv1 = tail_dv * ratio;

				pop_path.points.front() += ruis::vector2{dx, dv1};

				// it is possible that due to floating point calculation errors points
				// coincide
				if (pop_path.points.front().x() >= std::next(pop_path.points.begin())->x()) {
					pop_path.points.pop_front();
				}

				break;
			}
		}
		if (pop_path.points.size() <= 1) {
			pop_path.points.clear();
		}
	}

	// std::cout << "num_left = " << this->paths[0].points.size() << ", num_right
	// = " << this->paths[1].points.size() << std::endl;

	this->make_vaos();
}

void waveform::make_vaos()
{
	ASSERT(this->value_max > this->value_offset)
	const auto& height = this->rect().d.y() - 1; // -1 due to line width
	auto scale = height / (this->value_max - this->value_offset);

	for (auto& pv : this->paths) {
		if (pv.points.empty()) {
			pv.vao.set(ruis::path().stroke());
			continue;
		}

		pv.origin = {pv.points.front().x(), height - pv.points.front().y() * scale + this->value_offset};

		ruis::path path;
		for (const auto& p : utki::skip_front<1>(pv.points)) {
			ruis::vector2 point = {p.x(), height - p.y() * scale + this->value_offset};
			path.line_to(point - pv.origin);
		}

		constexpr auto waveform_line_halfwidth = 0.75;

		pv.vao.set(path.stroke(waveform_line_halfwidth));
	}
}
