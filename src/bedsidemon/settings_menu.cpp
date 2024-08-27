#include "settings_menu.hpp"

using namespace bedsidemon;

settings_menu::settings_menu(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context), //
		{
			.dims = {ruis::lp::fill, ruis::lp::fill}
},
		{}
	),
	ruis::nine_patch(
		this->context, //
		{
			.nine_patch_params =
				{
					.nine_patch = this->context.get().loader.load<ruis::res::nine_patch>("ruis_npt_window_bg") //
				} //
		},
		{}
	)
{}
