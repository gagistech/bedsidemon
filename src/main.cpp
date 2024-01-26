#include <ruisapp/application.hpp>

namespace bedsidemon{

class application : public ruisapp::application{

public:
    application() :
			ruisapp::application("ruis-tests", [](){
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));
		        return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file("../res"));
    }
};

const ruisapp::application_factory app_fac([](auto args){
	return std::make_unique<application>();
});

}
