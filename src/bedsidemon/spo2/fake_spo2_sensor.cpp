#include "fake_spo2_sensor.hpp"

#include <tml/tree.hpp>

#include "../application.hpp"

using namespace std::string_view_literals;

using namespace bedsidemon;

namespace {
std::vector<spo2_measurement> read_recorded_data()
{
	std::vector<spo2_measurement> ret;

	auto fi = bedsidemon::application::inst().get_res_file("spo2_measurements.tml");
	ASSERT(fi)

	auto frames = tml::read(*fi);

	for (const auto& f : frames) {
		if (f.value.to_string() != "frame"sv) {
			continue;
		}

		ret.push_back(spo2_measurement::from_tml(f.children));
	}

	return ret;
}
} // namespace

fake_spo2_sensor::fake_spo2_sensor(utki::shared_ref<spo2_parameter_window> pw) :
	spo2_sensor(std::move(pw)),
	record(read_recorded_data())
{}
