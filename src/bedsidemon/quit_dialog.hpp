#pragma once

#include "dialog.hpp"

namespace bedsidemon {

class quit_dialog : public ruis::dialog
{
public:
	quit_dialog(utki::shared_ref<ruis::context> context);
};

} // namespace bedsidemon
