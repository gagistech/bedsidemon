#pragma once

#include <ruis/util/timer.hpp>

#include "spo2_sensor.hpp"

namespace bedsidemon {

class fake_spo2_sensor : public spo2_sensor
{
	std::vector<spo2_measurement> record;

public:
	fake_spo2_sensor(utki::shared_ref<spo2_parameter_window> pw);

	fake_spo2_sensor(const fake_spo2_sensor&) = delete;
	fake_spo2_sensor& operator=(const fake_spo2_sensor&) = delete;

	fake_spo2_sensor(fake_spo2_sensor&&) = delete;
	fake_spo2_sensor& operator=(fake_spo2_sensor&&) = delete;

	~fake_spo2_sensor() override = default;

private:
};

} // namespace bedsidemon
