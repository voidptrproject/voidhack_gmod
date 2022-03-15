#include "features.hpp"

bool create_move_callback(float frametime, c_user_cmd* cmd) {
    

    return false;
}

void render_handler(render::render_data_t& data) {
    
}

static inline features::feature aimbot_feature([]() {
	hooks::add_listener(hooks::e_hook_type::create_move, create_move_callback);
    render::add_render_handler(render_handler);
});