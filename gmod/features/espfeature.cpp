#include "features.hpp"

#include <mutex>
#include <optional>

#include "../globals.hpp"
#include "../hooks.hpp"

#include <imgui_internal.h>

struct esp_box_t {
	ImVec2 min;
	ImVec2 max;

	inline auto width() const { return max.x - min.x; }
	inline auto height() const { return max.y - min.y; }

	inline ImVec2 center() const { return { min.x + width() / 2, min.y + height() / 2 }; }
};

struct esp_player_render_data_t {
	std::string user_group;
	bool is_admin;
};

struct esp_render_object_t {
	esp_box_t box;
	std::string name;
	int health;

	std::optional<esp_player_render_data_t> player_data = std::nullopt;

	c_color main_color;
	c_color name_color;
};

static std::vector<esp_render_object_t> esp_render_objects;
static std::mutex render_mutex;

inline bool get_entity_box(c_base_entity* ent, esp_render_object_t& render_object) {
	c_vector flb, brt, blb, frt, frb, brb, blt, flt;

	const auto& origin = ent->get_render_origin();
	const auto min = ent->get_collidable_ptr()->mins() + origin;
	const auto max = ent->get_collidable_ptr()->maxs() + origin;

	c_vector points[] = {
		c_vector(min.x, min.y, min.z),
		c_vector(min.x, max.y, min.z),
		c_vector(max.x, max.y, min.z),
		c_vector(max.x, min.y, min.z),
		c_vector(max.x, max.y, max.z),
		c_vector(min.x, max.y, max.z),
		c_vector(min.x, min.y, max.z),
		c_vector(max.x, min.y, max.z)
	};

	if (interfaces::debug_overlay->screen_position(points[3], flb) || interfaces::debug_overlay->screen_position(points[5], brt)
		|| interfaces::debug_overlay->screen_position(points[0], blb) || interfaces::debug_overlay->screen_position(points[4], frt)
		|| interfaces::debug_overlay->screen_position(points[2], frb) || interfaces::debug_overlay->screen_position(points[1], brb)
		|| interfaces::debug_overlay->screen_position(points[6], blt) || interfaces::debug_overlay->screen_position(points[7], flt))
		return false;

	c_vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	auto left = flb.x;
	auto top = flb.y;
	auto right = flb.x;
	auto bottom = flb.y;

	if (left < 0 || top < 0 || right < 0 || bottom < 0)
		return false;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	render_object.box.min.x = left;
	render_object.box.min.y = top;
	render_object.box.max.x = right;
	render_object.box.max.y = bottom;

	return true;
}

void esp_render_function(render::render_data_t& data) {
	std::lock_guard lock(render_mutex);

	for (auto& i : esp_render_objects) {
		auto name_size = render::calculate_text_size(i.name);

		data.draw_list->AddTextOutlined(ImGui::GetFont(),
			{ i.box.center().x - name_size.x / 2,
			  i.box.min.y - name_size.y},
			i.name_color, colors::black_color, i.name.c_str()
		);
		data.draw_list->AddTextOutlined(ImGui::GetFont(),
			{ i.box.center().x - render::calculate_text_size(i.player_data.value().user_group).x / 2,
			  i.box.max.y},
			i.player_data.value().is_admin ? colors::red_color : colors::white_color, colors::black_color, i.player_data.value().user_group.c_str()
		);

		data.draw_list->AddRect(i.box.min, i.box.max, i.main_color, 0.f, 0, 3.f);
		data.draw_list->AddRect(i.box.min - ImVec2(2.f, 2.f), i.box.max + ImVec2(2.f, 2.f), colors::black_color);
		data.draw_list->AddRect(i.box.min + ImVec2(2.f, 2.f), i.box.max - ImVec2(2.f, 2.f), colors::black_color);

		data.draw_list->AddRectFilledMultiColor({ i.box.max.x + 3, i.box.min.y + ((i.box.max.y - i.box.min.y) * ((100.f - i.health) / 100.f))}, 
			{i.box.max.x + 6, i.box.max.y}, colors::blue_color, colors::blue_color, colors::green_color, colors::green_color);
	}
	//esp_render_objects.clear();
}

void esp_update(const int stage) {
	if (stage != (int)e_frame_stage::frame_start || !settings::GetVariable<bool>("Esp")) {
		if (stage == (int)e_frame_stage::frame_start) {
			render_mutex.lock();
			esp_render_objects.clear();
			render_mutex.unlock();
		}
		return;
	}

	render_mutex.lock();
	esp_render_objects.clear();

	std::vector<esp_render_object_t> tmp;
	for (auto i = 0; i < interfaces::entity_list->get_highest_entity_index(); ++i) {
		auto entity = get_entity_by_index(i);
		if (!entity || !entity->is_alive() || entity->is_dormant())
			continue;
		if (entity->equal(get_local_player()) || !entity->is_player())
			continue;
		
		esp_render_object_t render_obj;
		if (!get_entity_box(entity, render_obj))
			continue;
		
		render_obj.health = std::clamp(entity->get_health_procentage(), 0, 100);
		render_obj.main_color = entity->as_player()->get_team_color();
		render_obj.name = entity->as_player()->get_name();
		
		esp_player_render_data_t player_data;
		player_data.user_group = entity->as_player()->get_user_group();
		player_data.is_admin = entity->as_player()->is_admin();

		render_obj.name_color = colors::white_color;
		render_obj.player_data.emplace(std::move(player_data));

		tmp.emplace_back(std::move(render_obj));
	}

	esp_render_objects.swap(tmp);
	render_mutex.unlock();
}

static inline features::feature esp_feature([]() {
	hooks::add_listener(hooks::e_hook_type::frame_stage_notify, esp_update);
	render::add_render_handler(esp_render_function);
});