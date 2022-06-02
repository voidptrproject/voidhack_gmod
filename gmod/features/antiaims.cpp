#include "features.hpp"

bool create_move_hook(float f, c_user_cmd* cmd) {
	return false;
}

void update_hook(e_frame_stage stage) {

}

void renderh(render::render_data_t& d) {

}

features::feature aaf([]() {
	hooks::add_listener(hooks::e_hook_type::create_move, create_move_hook);
	hooks::add_listener(hooks::e_hook_type::frame_stage_notify, update_hook);
	render::add_render_handler(renderh);
});