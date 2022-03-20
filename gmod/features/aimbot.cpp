#include "features.hpp"

std::mutex aimbot_mutex;
struct {
    c_vector position;
} aimbot_target;

auto lookup_bone(c_base_entity* ent, std::string_view bone_name) {
    static memory::symbol_t<int(*)(void*, const char*)> fn(memory::address_t({ "E8 ? ? ? ? 83 F8 FF 74 28" }, memory::client_module).absolute(0x1, 0x5));
    return fn.ptr(ent, bone_name.data());
}

auto get_target_position_for_entity(c_base_entity* entity) {
    auto bone = lookup_bone(entity, "ValveBiped.Bip01_Head1");
    return entity->get_bone(bone == -1 ? 0 : bone);
}

class aimbot_trace_filter : public i_trace_filter {
public:
    aimbot_trace_filter(void* local_player, void* target_entity) : lp(local_player), ent(target_entity) {}

    bool should_hit_entity(void* pEntityHandle, int contentsMask) {
        return pEntityHandle != lp && pEntityHandle != ent;
    }
    virtual trace_type_t get_trace_type() const {
        return trace_everything;
    }
    void* lp;
    void* ent;
};

auto can_do_damage(void* ent, const c_vector& shoot_pos) {
    aimbot_trace_filter filter(get_local_player(), ent);
    trace_t tr;
    utils::trace_ray(tr, get_local_player()->get_eye_pos(), shoot_pos, &filter);
    return tr.fraction == 1.f;
}

auto angle_to_entity(c_base_entity* entity) {
    q_angle output;

    const auto& target_position = get_target_position_for_entity(entity);
    const auto& position = get_local_player()->get_eye_pos();

    auto delta = position - target_position;
    auto hyp = delta.length2d();
    output.y = atanf(delta.y / delta.x) * (180.f / 3.14159265358979323846f);
    output.x = atanf(-delta.z / hyp) * -(180.f / 3.14159265358979323846f);

    if (delta.x >= 0.f)
        output.y += 180.f;

    output.normalize();

    return output;
}

c_base_entity* find_target() {
    return nullptr;
}

auto find_aimbot_target() -> c_base_player* {
    c_base_entity* outent = nullptr;
    float best_distance = FLT_MAX;
    for (auto i = 0; i < interfaces::entity_list->get_highest_entity_index(); ++i) {
        auto entity = get_entity_by_index(i);
        if (!entity || !entity->is_alive() || !entity->is_player() || entity->get_index() == get_local_player()->get_index())
            continue;
        auto shoot_position = get_target_position_for_entity(entity);
        if (can_do_damage(entity, shoot_position) && get_local_player()->get_origin().distance_to(shoot_position) < best_distance) {
            best_distance = get_local_player()->get_origin().distance_to(shoot_position);
            outent = entity;
        }
    }
    return (c_base_player*)outent;
}

auto angle_to_vector(const q_angle& angle) {
    c_vector out;
    math::angle_to_vector(angle, out);
    return out;
}

struct { bool shoot = false; c_vector position; bool draw = false; } current_target;

bool create_move_callback(float frametime, c_user_cmd* cmd) {
    auto target = find_aimbot_target();
    current_target.draw = false;

    if (!target)
        return false;

    current_target.shoot = false;
    current_target.draw = true;

    if (interfaces::debug_overlay->screen_position(get_target_position_for_entity(target), current_target.position)) {
        current_target.draw = false;
    }

    if (input::get_key_state(VK_MENU)) {
        cmd->viewangles = angle_to_entity(target);
        current_target.shoot = true;
    }

    return false;
}

void render_handler(render::render_data_t& data) {
    if (current_target.draw) {
        data.draw_list->AddLine({ ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y }, current_target.position, 
            current_target.shoot ? colors::red_color : colors::white_color);
    }
}

static inline features::feature aimbot_feature([]() {
    settings::CreateVariable("DrawAimBotTarget", false);

    menu::AddElementToCategory(menu::EMenuCategory_AimBot, std::make_shared<menu::ToggleButtonElement>("Draw AimBot Target", "DrawAimBotTarget"));

	hooks::add_listener(hooks::e_hook_type::create_move, create_move_callback);
    render::add_render_handler(render_handler);
});