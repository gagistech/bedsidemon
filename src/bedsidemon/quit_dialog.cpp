#include "quit_dialog.hpp"

#include <ruis/widget/label/text.hpp>
#include <ruis/widget/label/gap.hpp>
#include <ruis/widget/button/push_button.hpp>

using namespace std::string_literals;

using namespace ruis::length_literals;

using namespace bedsidemon;

namespace m{
using namespace ruis::make;
}

namespace{
constexpr auto dimension_buttons_gap = 10_pp;
}

namespace{
std::vector<utki::shared_ref<ruis::widget>> make_root_widget_structure(utki::shared_ref<ruis::context> c){
    auto make_button = [&](std::string id, std::u32string text){
        // clang-format off
        return m::push_button(c,
            {},
            {
                m::text(c,
                    {},
                    std::move(text)
                )
            }
        );
        // clang-format on
    };

    // clang-format off
    return {
        m::text(c,
            {},
            U"Quit program?"s
        ),
        m::row(c,
            {},
            {
                make_button("yet_button", U"Yes"s),
                m::gap(c,
                    {
                        .layout_params{
                            .dims = {dimension_buttons_gap, 0_px}
                        }
                    }
                ),
                make_button("no_button", U"No"s),
            }
        )
    };
    // clang-format on
}
}

quit_dialog::quit_dialog(utki::shared_ref<ruis::context> context) :
	ruis::widget(
		std::move(context), //
		{
			.dims{ruis::dim::max, ruis::dim::max}
},
		{}
	),
	ruis::dialog(
		this->context, //
		{
            .container_params{
                .layout = ruis::layout::column
            }
        },
		make_root_widget_structure(this->context)
	)
{}
