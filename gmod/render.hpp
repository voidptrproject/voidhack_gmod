#pragma once

#include "memory.hpp"

#include <d3d9.h>
#include <functional>
#include <string_view>

#include <imgui.h>

namespace render {
	struct render_handler;
	struct render_data_t;
	namespace internal {
		void render_hook(IDirect3DDevice9* device, const memory::address_t& return_address);
		void add_render_handler(render_handler* handler);
		void notify_render_handlers(render_data_t& context);

		struct render_context_t {
			ImDrawList* main_draw_list;
		};

		render_context_t& get_render_context();
	}

	struct render_data_t {
		ImDrawList* draw_list;
	};

	struct render_handler {
		using render_function_t = std::function<void(render_data_t& context)>;
		render_function_t render_function;

		render_handler(render_function_t rf) : render_function(rf) { internal::add_render_handler(this); }
	};
	
	IDirect3DDevice9* get_device();

	inline ImVec2 calculate_text_size(std::string_view text) { return ImGui::CalcTextSize(text.data()); }

}