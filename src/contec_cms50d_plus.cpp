#include "contec_cms50d_plus.hpp"

using namespace bedsidemon;

contec_cms50d_plus::contec_cms50d_plus() :
nitki::loop_thread(2)
{

}

contec_cms50d_plus::~contec_cms50d_plus(){

}

std::optional<uint32_t> contec_cms50d_plus::on_loop(){
    return {};
}