#include "render.hpp"

#include <mutex>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

#include "globals.hpp"
#include "input.hpp"

static inline IDirect3DDevice9* device;
static inline std::vector<render::render_handler> render_handlers;
static inline render::internal::render_context_t render_context;

render::internal::render_context_t& render::internal::get_render_context() {
	return render_context;
}

bool context_valid() {
	//return render_context.main_draw_list != nullptr;
	return true;
}

inline static input::key_handler menu_key_handler("menu_open", VK_INSERT, [&](input::e_key_state state) {
	auto& var = globals::get<bool>("menu_opened");
	if (state == input::e_key_state::released)
		var = !var;
});

struct override_render_state {
	DWORD original_value;
	D3DRENDERSTATETYPE state_type;
	IDirect3DDevice9* direct_device;

	override_render_state(IDirect3DDevice9* direct_device, D3DRENDERSTATETYPE state, DWORD override_value) : state_type(state), direct_device(direct_device) {
		direct_device->GetRenderState(state, &original_value);
		direct_device->SetRenderState(state, override_value);
	}
	~override_render_state() {
		direct_device->SetRenderState(state_type, original_value);
	}
};

void render::internal::render_hook(IDirect3DDevice9* dev, const memory::address_t& return_address) {
	static std::once_flag once_flag; std::call_once(once_flag, [&]() {
		if (auto hwnd = FindWindow(0, "Garry's Mod (x64)"); hwnd) {
			ImGui::CreateContext();
			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplDX9_Init(dev);
			ImGui::GetIO().IniFilename = 0;
		}
	});

	device = dev;
	if (!return_address.get_module().get_name().contains("gameoverlay")) return;

	render_data_t shared_render_data;
	override_render_state srgb_override(dev, D3DRS_SRGBWRITEENABLE, 0);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	shared_render_data.draw_list = ImGui::GetOverlayDrawList();
	notify_render_handlers(shared_render_data);

	if (globals::get<bool>("menu_opened")) {
		ImGui::GetIO().MouseDrawCursor = true;

		ImGui::Begin("TESTWINDOW");

		ImGui::InputInt("TESTINPUT", &settings::get<int>("testint"));
		ImGui::Checkbox("Bhop", &settings::get<bool>("bhop"));

		ImGui::End();
	} else {
		ImGui::GetIO().MouseDrawCursor = false;
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void render::internal::add_render_handler(render_handler* handler) {
	render_handlers.push_back(*handler);
}

void render::internal::notify_render_handlers(render_data_t& context) {
	std::for_each(render_handlers.begin(), render_handlers.end(), [&](render_handler& h) { h.render_function(context); });
}

IDirect3DDevice9* render::get_device() {
	return device;
}
