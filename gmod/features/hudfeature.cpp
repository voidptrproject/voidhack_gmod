#include "features.hpp"

#include <format>

struct observer_t {
	std::string observer_name;
	std::string target_name;
	int observing_mode;
};

static inline c_user_cmd last_cmd;
static inline std::string map_name;
static inline std::vector<observer_t> observers;

inline std::string current_time() {
	time_t now = time(NULL);
	struct tm tstruct;
	char buf[40];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);
	return buf;
}

void hud_render_function(render::render_data_t& data) {
	if (interfaces::engine->is_in_game()) {
		if (settings::GetVariable<bool>("InformationHUD")) {
			std::string server_ip = "localhost";
			float ping_outgoing, ping_ingoing = 0.f;
			if (auto net_channel = interfaces::engine->get_net_channel_info(); net_channel)
				server_ip = net_channel->get_ip_address(), ping_ingoing = net_channel->get_avg_latency(FLOW_OUTGOING) * 1000.f,
				ping_outgoing = net_channel->get_avg_latency(FLOW_INCOMING);

			ImGui::Begin("Information");
			ImGui::CopiedText("Time: %s", current_time().c_str());
			ImGui::CopiedText("Server: %s", server_ip.c_str());
			ImGui::CopiedText("Ping outgoing: %i", (uint32_t)(int)ping_outgoing);
			ImGui::CopiedText("Ping ingoing: %i", (uint32_t)(int)ping_ingoing);
			ImGui::CopiedText("Map name: %s", map_name.c_str());
			ImGui::CopiedText("Tickcount: %i", last_cmd.tick_count);
			ImGui::CopiedText("Buttons: %X", last_cmd.buttons);
			if (auto local_player = get_local_player(); local_player) {
				ImGui::Separator();
				ImGui::LabelText("##LOCALPLAYERTEXT", "LocalPlayer");
				ImGui::CopiedText("Pointer: 0x%X", (uintptr_t)local_player);
				ImGui::CopiedText("Flags: 0x%x", local_player->get_flags());
				ImGui::CopiedText("Health: %i", local_player->get_health());
				ImGui::CopiedText("Velocity: %f", local_player->get_velocity().length());
				ImGui::Separator();
			}
			ImGui::CopiedText(std::string("ViewAngles: ").append(last_cmd.viewangles.to_string_friendly()).c_str());
			ImGui::End();
		}

		if (settings::GetVariable<bool>("ObserversHUD")) {
			ImGui::Begin("Observers");
			for (auto& o : observers)
				ImGui::CopiedText("%s -> %s [%i]", o.observer_name.c_str(), o.target_name.c_str(), o.observing_mode);
			ImGui::End();
		}
	}
}

bool hud_render_create_move(float f, c_user_cmd* cmd) {
	if (settings::GetVariable<bool>("InformationHUD")) {
		last_cmd = *cmd;
		map_name = interfaces::engine->get_level_name();
	}

	if (settings::GetVariable<bool>("ObserversHUD")) {
		observers.clear();
		for (auto i = 0; i <= interfaces::entity_list->get_highest_entity_index(); ++i) {
			auto entity = get_player_by_index(i);
			if (!entity || !entity->is_player()) continue;
			if (entity->get_observer_mode() == 0) continue;

			observer_t tmp;
			tmp.observer_name = entity->get_name();
			tmp.target_name = entity->get_observer_target()->get_name();
			tmp.observing_mode = entity->get_observer_mode();

			observers.push_back(tmp);
		}
	}

	return false;
}

static inline features::feature hud_feature([]() {
	hooks::add_listener(hooks::e_hook_type::create_move, hud_render_create_move);
	render::add_render_handler(hud_render_function);
});