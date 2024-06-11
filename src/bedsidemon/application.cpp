#include "application.hpp"

#include <clargs/parser.hpp>

#include "gui.hpp"
#include "spo2/contec_cms50d_plus.hpp"
#include "spo2/setocare_st_t130_u01.hpp"
#include "spo2/spo2_parameter_window.hpp"

using namespace std::string_literals;

using namespace bedsidemon;

application::application(bool window, std::string_view res_path) :
		ruisapp::application( //
			"ruis-tests",
			[]() {
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 600));
				return wp;
			}()
		)
	{
		this->set_fullscreen(!window);

		this->gui.init_standard_widgets(*this->get_res_file());

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file(papki::as_dir(res_path)));

		auto c = make_root_widgets(this->gui.context);
		this->gui.set_root(c);

		auto& pw_container = c.get().get_widget_as<ruis::container>("pw_container");

		auto pw = utki::make_shared<spo2_parameter_window>(this->gui.context);
		// this->spo2_sensor_v = std::make_unique<contec_cms50d_plus>(pw, "/dev/ttyUSB0");
		// NOLINTNEXTLINE(bugprone-unused-return-value, "false positive")
		this->spo2_sensor_v = std::make_unique<setocare_st_t130_u01>(pw, "/dev/ttyUSB0");

		pw_container.push_back(pw);
	}

std::unique_ptr<application> bedsidemon::create_application(std::string_view executable, utki::span<const char*> args){
bool window = false;

	// TODO: look in /usr/local/share/bedsidemon first?
	std::string res_path = "/usr/share/bedsidemon"s;

	clargs::parser p;

	p.add("window", "run in window mode", [&]() {
		window = true;
	});

	p.add("res-path", "resources path, default = /usr/share/bedsidemon", [&](std::string_view v) {
		res_path = v;
	});

	p.parse(args);

	return std::make_unique<application>(window, res_path);
}
