#pragma once

#include <ruis/widget/label/nine_patch.hpp>

namespace bedsidemon {

class settings_menu : public ruis::nine_patch
{
public:
	settings_menu(utki::shared_ref<ruis::context> context);
};

} // namespace bedsidemon
