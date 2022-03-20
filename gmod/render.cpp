#include "render.hpp"

#include <mutex>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

#include "globals.hpp"
#include "input.hpp"
#include "menu/menu.hpp"

static inline IDirect3DDevice9* device;

inline auto& render_context() {
	static render::internal::render_context_t _render_context;
	return _render_context;
}

inline auto& render_handlers() {
	static std::vector<render::render_function_t> _render_handlers;
	return _render_handlers;
}

render::internal::render_context_t& render::internal::get_render_context() {
	return render_context();
}

bool context_valid() {
	//return render_context.main_draw_list != nullptr;				TODO: REMOVE THIS
	return true;
}

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
		ImGui::CreateContext();

		if (auto hwnd = FindWindow(0, "Garry's Mod (x64)"); hwnd) {
			ImGui_ImplWin32_Init(hwnd);
			ImGui_ImplDX9_Init(dev);
			ImGui::GetIO().IniFilename = "garrysmod\\cfg\\gmoduser_.txt";

			static const ImWchar ranges[] = {
				0x0020, 0x00FF, // Basic Latin + Latin Supplement
				0x0400, 0x044F, // Cyrillic
				0,
			};

			//ImGui::StyleColorsDark();

			ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/Windows/Fonts/L_10646.ttf", 18.f, 0, ranges);
		}
	});

	device = dev;

	if (!(return_address.get_module().get_name().find("gameoverlay") == return_address.get_module().get_name().npos)) return;

	render_data_t shared_render_data;
	override_render_state srgb_override(dev, D3DRS_SRGBWRITEENABLE, 0);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	notifymanager::GetNotifyManager().UpdateAndRender();

	menu::render_menu();

	shared_render_data.draw_list = ImGui::GetOverlayDrawList();
	notify_render_handlers(shared_render_data);

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void render::add_render_handler(render::render_function_t handler) {
	render_handlers().push_back(handler);
}

void render::internal::notify_render_handlers(render_data_t& context) {
	std::for_each(render_handlers().begin(), render_handlers().end(), [&](render_function_t h) { if (h) h(context); });
}

IDirect3DDevice9* render::get_device() {
	return device;
}

notifymanager::NotifyManager& notifymanager::GetNotifyManager() {
	static NotifyManager nmgr;
	return nmgr;
}

void notifymanager::AddNotify(std::string_view data) {
	GetNotifyManager().GetMutex().lock();
	GetNotifyManager().AddNotify({data, data});
	GetNotifyManager().GetMutex().unlock();
}
