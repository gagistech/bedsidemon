#include <ruisapp/application.hpp>

#include "contec_cms50d_plus.hpp"

namespace bedsidemon{

class application : public ruisapp::application{
	contec_cms50d_plus spo2_sensor;
public:
    application() :
			ruisapp::application("ruis-tests", [](){
				// NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
				ruisapp::window_params wp(r4::vector2<unsigned>(1024, 800));
		        return wp;
			}())
	{
		this->gui.init_standard_widgets(*this->get_res_file());

		this->gui.context.get().loader.mount_res_pack(*this->get_res_file("res/"));

		auto c = this->gui.context.get().inflater.inflate(
				*this->get_res_file("res/main.gui")
			);
		this->gui.set_root(c);
    }
};

const ruisapp::application_factory app_fac([](auto args){
	return std::make_unique<application>();
});

}
