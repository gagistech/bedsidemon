#include "gui.hpp"

using namespace std::string_literals;

using namespace bedsidemon;

utki::shared_ref<ruis::widget> bedsidemon::make_root_widgets(utki::shared_ref<ruis::context> c)
{
	namespace m = ruis::make;
	using ruis::lp;

	return
		// clang-format off
		m::container(
			c,
			{
				.widget_params = {
					.id = "pw_container"s
				},
				.container_params = {
					.layout = ruis::layout::column
				}
			},
			{}
		);
	// clang-format on
}
