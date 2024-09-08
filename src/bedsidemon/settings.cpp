#include "settings.hpp"

#include <filesystem>

#include <papki/fs_file.hpp>
#include <ruisapp/application.hpp>

using namespace std::string_view_literals;

using namespace bedsidemon;

namespace {
constexpr const auto settings_filename = "settings.tml"sv;

constexpr const auto sweep_speed_key = "sweep_speed_um_per_sec"sv;
} // namespace

settings_storage::settings_storage() :
	filename(utki::cat(ruisapp::application::inst().directory.config, settings_filename)),
	settings_v(read(this->filename))
{}

settings settings_storage::read(std::string_view filename)
{
	papki::fs_file fi(filename);

	if (!fi.exists()) {
		LOG([](auto& o) {
			o << "settings file not found, use default settings" << std::endl;
		})
		return {};
	}

	settings ret;

	auto tml = tml::read(fi);

	for (const auto& t : tml) {
		if (t.value.to_string() == sweep_speed_key) {
			// TODO:
		}
	}

	return ret;
}

void settings_storage::set(const settings& s)
{
	this->settings_v = s;
	this->write();
}

void settings_storage::write()
{
	settings default_values;

	tml::forest tml;

	auto add_setting = [&tml](std::string_view key, tml::leaf val) {
		tml.push_back(tml::tree(key, {val}));
	};

	if (this->settings_v.sweep_speed_um_per_sec != default_values.sweep_speed_um_per_sec) {
		add_setting(sweep_speed_key, tml::leaf(this->settings_v.sweep_speed_um_per_sec));
	}

	if (tml.empty()) {
		return;
	}

	std::filesystem::create_directories(ruisapp::application::inst().directory.config);

	papki::fs_file fi(filename);
	tml::write(tml, fi);
}
