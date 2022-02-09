#include "render.hpp"

void render::internal::render_hook(const memory::address_t& return_address) {
	if (!return_address.get_module().get_name().contains("gameoverlay")) return;


}
