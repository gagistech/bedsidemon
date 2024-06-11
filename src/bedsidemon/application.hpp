#pragma once

#include <ruisapp/application.hpp>

#include "spo2/spo2_sensor.hpp"

namespace bedsidemon {

class application : public ruisapp::application
{
	std::unique_ptr<spo2_sensor> spo2_sensor_v;

public:
	application(bool window, std::string_view res_path);
};

std::unique_ptr<application> create_application(std::string_view executable, utki::span<const char*> args);

} // namespace bedsidemon
